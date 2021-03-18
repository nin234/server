#include <MessageProcessor.h> 
#include <ShareIdMgr.h>
#include <ArchiveMsgCreator.h>
#include <MessageObjs.h>
#include <Constants.h>
#include <iostream>
#include <sstream>
#include <functional>
#include <FrndLstMgr.h>
#include <algorithm>
#include <Util.h>
#include <Config.h>
#include <DistribMgr.h>

using namespace std::placeholders;

MessageProcessor::MessageProcessor():m_pDcd(NULL), m_pTrnsl(NULL), m_pPicSndr(NULL),  pNtwIntf(new NtwIntf<MessageDecoder>()), pArch(new ArchiveSndr()), 
      pMsgEnq(new MessageEnqueuer()), m_distributor(new Distributor()),
      dataStore{CommonDataMgr::Instance()}
{
	nFds = 0;
    pthread_t tid;
    if (pthread_create(&tid, NULL, &Distributor::entry, m_distributor.get()) != 0)
    {
        std::cout << "Failed to create Worker thread " << __FILE__ << ":" << __LINE__ << std::endl;
        throw std::system_error(errno, std::system_category());			
    }
}

MessageProcessor::~MessageProcessor()
{

}

bool
MessageProcessor::process()
{
	std::cout << "Start processing of events " << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	for (;;)
	{
        getSendEvents();
		if (pNtwIntf->waitAndGetMsg())	
		{
			processRequests();
			m_pPicSndr->sendPictures();
		}
	}
	return false;
}
bool
MessageProcessor::addFd(int fd)
{
	if (nFds > maxFd)	
	{
		std::cout << "Exceeded max capacity in worker fds nFds=" << nFds << " maxFd=" << maxFd << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		close(fd);
		return false;
	}
	if (pNtwIntf->addFd(fd))
		++nFds;
	else 
		return false;
	return true;

}

bool
MessageProcessor::addSSLFd(int fd)
{
	if (nFds > maxFd)	
	{
		std::cout << "Exceeded max capacity in worker fds nFds=" << nFds << " maxFd=" << maxFd << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		close(fd);
		return false;
	}
	if (pNtwIntf->addSSLFd(fd))
		++nFds;
	else 
		return false;
	return true;

}

void
MessageProcessor::setMaxFd(int mfd)
{
	maxFd = mfd;
}

void
MessageProcessor::setDcdTransl(MessageDecoder *pDcd, MessageTranslator *pTrnsl)
{
	m_pDcd.reset(pDcd);
	m_pPicSndr = std::make_shared<PictureSender>();
	m_pTrnsl.reset(pTrnsl);
	pNtwIntf->setDecoder(m_pDcd);
	m_pPicSndr->setTrnsl(pTrnsl);
	m_pPicSndr->attach(this);
	pNtwIntf->attach(this);
    m_distributor->setTrnsl(pTrnsl);
	return;
}

void
MessageProcessor::onCloseFd(int fd)
{
	std::vector<PicFileDetails> pfdVec = m_pPicSndr->onCloseNtwFd(fd);
	for(const auto& pfd : pfdVec)
	{
		std::cout << "Updating picSharestatus for " << pfd<< std::endl;
        dataStore.updatePicShareStatus(pfd.appId, pfd.shareId, pfd.frndShareId, pfd.picName);
        
	}
	--nFds;
}


void
MessageProcessor::processMsg(std::shared_ptr<MsgObj> pMsg, int nMsgTyp)
{
	switch(nMsgTyp)
	{
		case GET_SHARE_ID_MSG:
			return processShareIdMsg(pMsg);	

		case STORE_TRNSCTN_ID_MSG:
			return processStoreIdMsg(pMsg);
		case STORE_FRIEND_LIST_MSG:
			return processFrndLstMsg(pMsg);
		case ARCHIVE_ITEM_MSG:
			return processArchvItemMsg(pMsg);
		case SHARE_ITEM_MSG:
        {
            if (Config::Instance().useDB())
            {
			    return processItemMsgDBInsert(pMsg);
            }
            else
            {
			    return processItemMsg(pMsg);
            }
        }    
		case STORE_DEVICE_TKN_MSG:
			return processDeviceTknMsg(pMsg);
		case GET_ITEMS:
        {
			    return processGetItemMsgFromDB(pMsg);
        }
		case PIC_METADATA_MSG:
			return processPicMetaDataMsg(pMsg);
		case PIC_MSG:
			return processPicMsg(pMsg);
		case PIC_DONE_MSG:
			return processPicDoneMsg(pMsg);
		case SHOULD_DOWNLOAD_MSG:
			return processShouldDownLoadMsg(pMsg);

        case GET_REMOTE_HOST_MSG:
            return processGetRemoteHostMsg(pMsg);
	
		default:
			std::cout << "Unhandled message type=" << nMsgTyp << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		break;
	}	

	return;
}

void
MessageProcessor::processRequests()
{
		for (;;)
	{
		std::shared_ptr<MsgObj> pMsg{m_pDcd->getNextMsg()};
		if (!pMsg)
			break;
		int nMsgTyp = pMsg->getMsgTyp();
		if (nMsgTyp < 0)
			continue;
		processMsg(pMsg);
		if (nMsgTyp != SHOULD_DOWNLOAD_MSG && m_pPicSndr->shouldEnqueMsg(pMsg->getFd()))
		{
			pMsgEnq->enqMsg(pMsg);	
			continue;
		}
		processMsg(pMsg, nMsgTyp);						
	}
	return;
}

    
    void
    MessageProcessor::processGetRemoteHostMsg(std::shared_ptr<MsgObj> pMsg)
    {
        auto pGetRemoteHostObj = std::dynamic_pointer_cast<GetRemoteHostObj>(pMsg);
        if (!pGetRemoteHostObj)
        {
            std::cout << Util::now() <<  "Invalid message received in MessageProcessor::processGetRemoteHostMsg " << " " << __FILE__ << ":" << __LINE__ << std::endl;   
            return;
        }

        std::cout << "GET_REMOTE_HOST_MSG with shareId=" << pGetRemoteHostObj->getShareId() << " appId=" << pGetRemoteHostObj->getAppId() << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        std::pair<std::string, int> hostPort;
        if (DistribMgr::Instance().getNode(pGetRemoteHostObj->getShareId(), pGetRemoteHostObj->getAppId(), hostPort))
        {
            char buf[32768];
            int mlen=0;
            if (m_pTrnsl->getShareIdRemoteHostMsg(buf, &mlen, hostPort.first, hostPort.second))
            {
                if (!pNtwIntf->sendMsg(buf, mlen, pGetRemoteHostObj->getFd()))
                    std::cout << "Failed to send message for fd=" << pGetRemoteHostObj->getFd() << std::endl;
                else
                    std::cout << Util::now() << "Replying with host=" << hostPort.first 
                    << " port=" << hostPort.second << " " << __FILE__ << " " << __LINE__ << std::endl;
            }
        }
    }

    void
    MessageProcessor::processShouldDownLoadMsg(std::shared_ptr<MsgObj> pMsg)
    {
        auto pShouldDownLoad  = std::dynamic_pointer_cast<ShouldDownLoad>(pMsg);
        if (!pShouldDownLoad )
        {
            std::cout << "Invalid message received in MessageProcessor::processShouldDownLoadMsg " << " " << __FILE__ << ":" << __LINE__ << std::endl;	
            return;
        }
	std::cout << "Received ShouldDownLoad message " << *pShouldDownLoad << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	m_pPicSndr->updateWaitingInfo(pShouldDownLoad->getName(), pShouldDownLoad->getShrId(), pShouldDownLoad->downLoad());
    }

void
MessageProcessor::processPicMsg(std::shared_ptr<MsgObj> pMsg)
{
	auto pPicObj = std::dynamic_pointer_cast<PicObj>(pMsg);
	if (!pPicObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicMsg " << std::endl;
		return;
	}
	//std::cout << "Storing picture object " << __FILE__ << ":" << __LINE__ << std::endl;
    bool cleanUpFd;
	if (dataStore.storePic(pPicObj, cleanUpFd))
	{
		std::cout << "Stored picture object " << __FILE__ << ":" << __LINE__ << std::endl;
		//send push notification
		std::vector<std::string> shareIds = dataStore.getPicShareIds(pPicObj->getFd()); 
		if (shareIds.size())
		{
			
			std::string picName = dataStore.getPicName(pPicObj->getFd());
			std::cout << "Sending push notification to receive picture name=" << picName << " " << __FILE__ << ":" << __LINE__ << std::endl;
			sendPicNotifications(shareIds, pPicObj->getAppId(), picName);
		}
	    m_distributor->distributePicture(dataStore.getPicMetaObj(pPicObj->getFd()));
		dataStore.eraseFdMp(pPicObj->getFd());
	
	}
    else
    {
        if (cleanUpFd)
        {
            pNtwIntf->closeAndCleanUpFd(pPicObj->getFd());
        }
    }
	return;
}

void
MessageProcessor::sendPicNotifications(const std::vector<std::string>& shareIds, int appId, const std::string& picName)
{
	std::vector<std::string> tokens;
	dataStore.getDeviceTkns(appId, shareIds, tokens);	
	std::cout << "Sending push notification to receive picture name=" << picName << " " << __FILE__ << ":" << __LINE__ << std::endl;
	if (tokens.size())
	{
		sendApplePush(appId, tokens, picName, 1);
	}
	std::vector<std::string> regIds;
	dataStore.getAndroidDeviceTkns(appId, shareIds, regIds);
	if (regIds.size())
	{
		sendFirebaseMsg(appId, regIds, picName);
	}
}

void
MessageProcessor::processPicMetaDataMsg(const std::shared_ptr<MsgObj> pMsg)
{

	auto pPicMetaObj = std::dynamic_pointer_cast<PicMetaDataObj>(pMsg);
	if (!pPicMetaObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicMetaDataMsg " << std::endl;
		return;
	}
	std::cout << "Received " << *pPicMetaObj << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	m_distributor->distribute(pPicMetaObj);
	int picOffset = 0;
	bool bShoulUpload = dataStore.storePicMetaData(pPicMetaObj, &picOffset);
	const std::vector<std::string>&  shareIds = pPicMetaObj->getFrndLst();
    
	char buf[32768];
	int mlen=0;
	if (m_pTrnsl->getShouldUploadMsg(buf, &mlen, pPicMetaObj, bShoulUpload, picOffset))
	{
		sendMsg(buf, mlen, pPicMetaObj->getFd());
	}

	if (!bShoulUpload && shareIds.size())
	{
		sendPicNotifications(shareIds, pPicMetaObj->getAppId(), pPicMetaObj->getName());
	}
	return;
}

void
MessageProcessor::processPicDoneMsg(std::shared_ptr<MsgObj> pMsg)
{
	auto pPicDoneObj = std::dynamic_pointer_cast<PicDoneObj>(pMsg);
	if (!pPicDoneObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicDoneMsg " << std::endl;
		return;
	}
	std::cout << "Received PicDone Msg=" << *pPicDoneObj << " " << __FILE__ << ":" << __LINE__ << std::endl;
	updatePicShareInfo(pPicDoneObj->getAppId(), pPicDoneObj->getShrId(), pPicDoneObj->getPicShareId(), pPicDoneObj->getPicName());
    if (!Config::Instance().useDB())
    {
        char archbuf[32768];
        int archlen = 0;
        std::cout << "Updating Archive with picDoneMsg in processPicDoneMsg " << __FILE__ << ":" << __LINE__ << std::endl;
        if (ArchiveMsgCreator::createPicMetaDataMsg(archbuf, archlen, pPicDoneObj->getAppId(), true, pPicDoneObj->getShrId(), pPicDoneObj->getPicShareId(), pPicDoneObj->getPicName(),  32768, 100))
            sendArchiveMsg(archbuf, archlen, 10);	
    }


}

void
MessageProcessor::sendUpdatedMaxShareIdIfRequd(std::shared_ptr<GetItemObj> pGetItemObj)
{
    long maxAppShareId = pGetItemObj->getMaxShareId();
    long maxShareId = ShareIdMgr::Instance().readShareId();
    if (!maxAppShareId)
    {
        return;
    }

    if (maxAppShareId - maxShareId < 500)
    {
        long newMaxAppShareId = 0;
        if (maxAppShareId >= maxShareId)
        {
            newMaxAppShareId = maxAppShareId + 3000;    
        }
        else
        {
            newMaxAppShareId = maxShareId + 3000;
        }
		char buf[512];
		int mlen=0;
		if (m_pTrnsl->getUpdateShareIdMsg(buf, &mlen, newMaxAppShareId))
		{
			if (!pNtwIntf->sendMsg(buf, mlen, pGetItemObj->getFd()))
				std::cout << "Failed to send message for fd=" << pGetItemObj->getFd() << std::endl;
            else
		        std::cout << Util::now() << "Updating max shareId=" << newMaxAppShareId << " " << __FILE__ << " " << __LINE__ << std::endl;
		}



    }
    return;
}

void
MessageProcessor::processGetItemMsgFromDB(std::shared_ptr<MsgObj> pMsg)
{
	auto pGetItemObj = std::dynamic_pointer_cast<GetItemObj>(pMsg);
	if (!pGetItemObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processGetItemMsg " << std::endl;
		return;
	}

    sendUpdatedMaxShareIdIfRequd(pGetItemObj);
    sendFrndLst(pGetItemObj->getAppId(), pGetItemObj->getShrId(), pGetItemObj->getFd(), false);

	std::cout << Util::now() << "Received GetItem Msg=" << *pGetItemObj << " " << __FILE__ << ":" << __LINE__ << std::endl;

	std::map<shrIdLstName, std::string> lstNameMp;
	dataStore.getShareLists(pGetItemObj->getAppId(), pGetItemObj->getShrId(), lstNameMp);
	char archbuf[32768];
	int archlen = 0;
	for (auto pItr = lstNameMp.begin(); pItr != lstNameMp.end(); ++pItr)
	{
		std::cout << " Send item details " << pItr->first << " " << pItr->second << " " << __FILE__ << ":" << __LINE__ << std::endl;
		if (m_pTrnsl->getListMsg(archbuf, &archlen, 32768, pItr->first.lstName, pItr->second, SHARE_ITEM_MSG, pItr->first.shareId))
		{
			if (sendMsg(archbuf, archlen, pGetItemObj->getFd()))
			{
				std::cout << "Sent Item=" << pItr->first.lstName << " from shareId=" << pItr->first.shareId <<" to shareId=" << pGetItemObj->getShrId() << " length=" << archlen << " " << __FILE__ << ":" << __LINE__ << std::endl;
			}
            else
            {
				std::cout << "Failed to sent Item=" << pItr->first.lstName << " from shareId=" << pItr->first.shareId <<" to shareId=" << pGetItemObj->getShrId() << " length=" << archlen << " " << __FILE__ << ":" << __LINE__ << std::endl;
                return;

            }
		}
	}
	dataStore.delShareLists(pGetItemObj->getAppId(), pGetItemObj->getShrId());
    
    processGetItemPics(pGetItemObj);
    return;
}

void
MessageProcessor::sendFrndLst(int appId, long shareId, int fd, bool bDontCheckUpdFlag)
{
    std::string frndLst = dataStore.getFrndList(appId, shareId, bDontCheckUpdFlag);
    if (!frndLst.size())
        return;
    char archbuf[32768];
    int archlen = 0;
    if (m_pTrnsl->getFrndLstMsg(archbuf, &archlen, 32768, frndLst))
    {
        if (sendMsg(archbuf, archlen, fd))
        {
            std::cout << "Send friend list=" << frndLst << " to shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;    
            dataStore.updateFrndLstStatus(shareId, appId);
        }
        else
        {
            std::cout << "Failed to send friend list=" << frndLst << " to shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;    

        }
    }
}


    
void
MessageProcessor::processGetItemPics(std::shared_ptr<GetItemObj> pGetItemObj)
{
    std::vector<shrIdLstName> picNamesShIds;
    dataStore.getPictureNames(pGetItemObj->getAppId(), pGetItemObj->getShrId(), picNamesShIds);
    long nTotalDownLoadSize = 0;

    if (pGetItemObj->getPicRemaining())
    {
        auto pItr = find_if(picNamesShIds.begin(), picNamesShIds.end(), [pGetItemObj](const shrIdLstName& shItem) {return shItem.lstName.find(pGetItemObj->getPicName()) != std::string::npos && shItem.shareId == pGetItemObj->getPicShareId();});
        if (pItr != picNamesShIds.end())
        {
	    std::cout << "Found partially downloaded item=" << *pItr << " " << __FILE__ << ":" << __LINE__ << std::endl;	
            (*pItr).picSoFar = (*pItr).picLen - pGetItemObj->getPicRemaining();
            (*pItr).fd = pGetItemObj->getFd();
            if ((*pItr).picSoFar < 0)
                (*pItr).picSoFar = 0;
            nTotalDownLoadSize += (*pItr).picLen - (*pItr).picSoFar;
            m_pPicSndr->insertPicNameShid(*pItr);
        }
    }
    
    for (auto& picNameShId : picNamesShIds)
    {
        if (pGetItemObj->getPicRemaining() && picNameShId.lstName.find(pGetItemObj->getPicName()) != std::string::npos && pGetItemObj->getPicShareId() == picNameShId.shareId)
        {
            continue;
        }
        picNameShId.fd = pGetItemObj->getFd();
        nTotalDownLoadSize += picNameShId.picLen;
        m_pPicSndr->insertPicNameShid(picNameShId);
    }
    if (!nTotalDownLoadSize)
    {
        return;
    }
	char buf[512];
	int mlen=0;
    if (m_pTrnsl->getTotalPicLenMsg(buf, &mlen, nTotalDownLoadSize))
    {
        if (!pNtwIntf->sendMsg(buf, mlen, pGetItemObj->getFd()))
            std::cout << Util::now() << "Failed to send TOTAL_PIC_LEN_MSG message for fd=" << pGetItemObj->getFd() << std::endl;
        else
            std::cout << Util::now() << "Sending TOTAL_PIC_LEN_MSG  download size=" << nTotalDownLoadSize << " " << __FILE__ << " " << __LINE__ << std::endl;
    }


    
}

void
MessageProcessor::processItemMsgDBInsert(std::shared_ptr<MsgObj> pMsg)
{
	auto pLstObj = std::dynamic_pointer_cast<LstObj>(pMsg);
	if (!pLstObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processItemMsg " << std::endl;
		return;
	}
	std::cout << "Received Item message appId="<< pLstObj->getAppId() << " shareId=" << pLstObj->getShrId() << " name=" << pLstObj->getName() << " item=" << pLstObj->getList() << " " << __FILE__ << ":" << __LINE__  << std::endl;
	std::string lstToStore;
	std::string::size_type xpos = pLstObj->getList().find(":::");
	if (xpos == std::string::npos)
	{
		std::cout << "Invalid item received item=" << pLstObj->getList() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return;
	}
	lstToStore = pLstObj->getList().substr(xpos+3);
	std::vector<std::string>  shareIds = m_distributor->distribute(pLstObj);
	if (shareIds.size())
	{
		dataStore.storeItemAndLstShareInfo(pLstObj->getAppId(),pLstObj->getShrId(), pLstObj->getName(), lstToStore, shareIds);
		std::vector<std::string> tokens;
		dataStore.getDeviceTkns(pLstObj->getAppId(), shareIds, tokens);	
		if (tokens.size())
			sendApplePush(pLstObj->getAppId(), tokens, pLstObj->getName(), 1);
        std::vector<std::string> regIds;
        dataStore.getAndroidDeviceTkns(pLstObj->getAppId(), shareIds, regIds);
	std::cout << "Sending push notifications no of tokens=" << tokens.size() << " no of regIds=" << regIds.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;
        if (regIds.size())
        {
                sendFirebaseMsg(pLstObj->getAppId(), regIds, pLstObj->getName());
        }
            
	}
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, SHARE_ITEM_RPLY_MSG))
	{
		sendMsg(buf, mlen, pLstObj->getFd());
	}

	
	return;
}


void
MessageProcessor::processItemMsg(std::shared_ptr<MsgObj> pMsg)
{
	auto pLstObj = std::dynamic_pointer_cast<LstObj>(pMsg);
	if (!pLstObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processItemMsg " << std::endl;
		return;
	}
	std::cout << "Received Item message appId="<< pLstObj->getAppId() << " shareId=" << pLstObj->getShrId() << " name=" << pLstObj->getName() << " item=" << pLstObj->getList() << " " << __FILE__ << ":" << __LINE__  << std::endl;
	std::string lstToStore;
	std::string::size_type xpos = pLstObj->getList().find(":::");
	if (xpos == std::string::npos)
	{
		std::cout << "Invalid item received item=" << pLstObj->getList() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return;
	}
	lstToStore = pLstObj->getList().substr(xpos+3);
	dataStore.storeItem(pLstObj->getAppId(), pLstObj->getShrId(), pLstObj->getName(), lstToStore);	
	
	//int size= 

	char archbuf[32768];
	int archlen = 0;
	if (ArchiveMsgCreator::createItemMsg(archbuf, archlen, pLstObj->getAppId(), pLstObj->getShrId(), pLstObj->getName(), lstToStore, 32768))
		sendArchiveMsg(archbuf , archlen, 10);	

	std::vector<std::string>  shareIds;
	if (m_pTrnsl->getShareIds(pLstObj->getList(), shareIds))
	{
		dataStore.storeLstShareInfo(pLstObj->getAppId(),pLstObj->getShrId(), shareIds, pLstObj->getName());
		for (const std::string& shareId : shareIds)
		{
			std::cout << "Archiving shareId=" << shareId << " in processItemMsg " << __FILE__ << ":" << __LINE__ << std::endl;
		
			if (ArchiveMsgCreator::createShareLstMsg(archbuf, archlen, pLstObj->getAppId(), false, std::stol(shareId), pLstObj->getShrId(), pLstObj->getName(),  32768))
				sendArchiveMsg(archbuf, archlen, 10);	
		}
		std::vector<std::string> tokens;
		dataStore.getDeviceTkns(pLstObj->getAppId(), shareIds, tokens);	
		if (tokens.size())
			sendApplePush(pLstObj->getAppId(), tokens, pLstObj->getName(), 1);
        std::vector<std::string> regIds;
        dataStore.getAndroidDeviceTkns(pLstObj->getAppId(), shareIds, regIds);
	std::cout << "Sending push notifications no of tokens=" << tokens.size() << " no of regIds=" << regIds.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;
	if (regIds.size())
	{
        	sendFirebaseMsg(pLstObj->getAppId(), regIds, pLstObj->getName());
	}
        
	}
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, SHARE_ITEM_RPLY_MSG))
	{
		sendMsg(buf, mlen, pLstObj->getFd());
	}

	
	return;
}

void
MessageProcessor::processArchvItemMsg(std::shared_ptr<MsgObj> pMsg)
{
	auto pTmplObj = std::dynamic_pointer_cast<TemplLstObj>(pMsg);
	if (!pTmplObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processArchvItemMsg " << std::endl;
		return;
	}
	dataStore.storeArchiveItem(pTmplObj->getAppId(), pTmplObj->getShrId(), pTmplObj->getName(), pTmplObj->getTemplList());	
	
	//int size= 

	std::unique_ptr<char> pArchMsg;
	char archbuf[32768];
	int archlen = 0;
	pArchMsg = ArchiveMsgCreator::createArchvItmMsg(archbuf, archlen, pTmplObj->getShrId(), pTmplObj->getName(), pTmplObj->getTemplList(), 32768);
	sendArchiveMsg(pArchMsg == nullptr?archbuf : pArchMsg.get(), archlen, 10);	
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, ARCHIVE_ITEM_RPLY_MSG))
	{
		sendMsg(buf, mlen, pTmplObj->getFd());
	}
	return;
}

void
MessageProcessor::processDeviceTknMsg(std::shared_ptr<MsgObj> pMsg)
{
	auto pDevTknObj = std::dynamic_pointer_cast<DeviceTknObj>(pMsg);
	if (!pDevTknObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processDeviceTknMsg " << std::endl;
		return;
	}
	dataStore.storeDeviceTkn(pDevTknObj->getAppId(), pDevTknObj->getShrId(), pDevTknObj->getDeviceTkn(), pDevTknObj->getPlatform());
    
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, STORE_DEVICE_TKN_RPLY_MSG))
	{
		std::cout << "Send reply STORE_DEVICE_TKN_RPLY_MSG" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		sendMsg(buf, mlen, pDevTknObj->getFd());
	}
		
	return;
}

void
MessageProcessor::processShareIdLocal(std::shared_ptr<MsgObj> pMsg)
{
	auto pShObj = std::dynamic_pointer_cast<ShareIdObj>(pMsg);

	long shareId = 0;
	if (pShObj)
		shareId = ShareIdMgr::Instance().getShareId(pShObj->getDeviceId());
	if (shareId)
	{
		
		//send reply
		char buf[512];
		int mlen=0;
		if (m_pTrnsl->getShareIdReply(buf, &mlen, shareId))
		{
			if (!pNtwIntf->sendMsg(buf, mlen, pShObj->getFd()))
				std::cout << "Failed to send message for fd=" << pShObj->getFd() << std::endl;
            else
		        std::cout << Util::now() << "Replying with shareId=" << shareId << " " << __FILE__ << " " << __LINE__ << std::endl;
		}

        sendFrndLst(pShObj->getAppId(), shareId, pShObj->getFd(), true);
	}
	return;

}

void
MessageProcessor::processShareIdMsg(std::shared_ptr<MsgObj> pMsg)
{
	auto pShObj = std::dynamic_pointer_cast<ShareIdObj>(pMsg);
	if (pShObj)
	{
        auto [host, port] = DistribMgr::Instance().getNewShareIdHost(pShObj->getAppId()); 
        if (host == "LOCAL")
        {
            processShareIdLocal(pMsg);
            return;
        }
		//send reply
		char buf[32768];
		int mlen=0;
		if (m_pTrnsl->getShareIdRemoteHostMsg(buf, &mlen, host, port))
		{
			if (!pNtwIntf->sendMsg(buf, mlen, pShObj->getFd()))
				std::cout << "Failed to send message for fd=" << pShObj->getFd() << std::endl;
            else
		        std::cout << Util::now() << "Replying with host=" << host 
                << " port=" << port << " " << __FILE__ << " " << __LINE__ << std::endl;
		}

	}
	return;

}

void
MessageProcessor::processStoreIdMsg(std::shared_ptr<MsgObj> pMsg)
{
		return;
}

void
MessageProcessor::processFrndLstMsg(std::shared_ptr<MsgObj> pMsg)
{
	auto pFrndObj = std::dynamic_pointer_cast<FrndLstObj>(pMsg) ;
	if (!pFrndObj)
		return;
	std::cout << "Received friend list message shareId=" << pFrndObj->getShrId() << " friend list=" << pFrndObj->getFrndLst() <<  " " << __FILE__ << ":" << __LINE__  << std::endl;
    dataStore.storeFrndLst(pFrndObj);
	char buf[512];
	int mlen =0;
	if (m_pTrnsl->getReply(buf, &mlen, STORE_FRIEND_LIST_RPLY_MSG))
	{
		if (!pNtwIntf->sendMsg(buf, mlen, pFrndObj->getFd()))
			std::cout << "Failed to send message for fd=" << pFrndObj->getFd() << std::endl;
	}

	return;
}

void
MessageProcessor::sendArchiveMsg(const char *pMsg, size_t len, unsigned int msg_prio)
{
	pArch->sendMsg(pMsg, len, msg_prio);
	return;
}

bool
MessageProcessor::notify(char *buf, int mlen, int fd, bool *tryAgain)
{
	return sendMsg(buf, mlen , fd, tryAgain);
}

void
MessageProcessor::updatePicShareInfo(int appId, long shareId, long frndShareId, const std::string& picName)
{
	dataStore.updatePicShareInfo(appId, shareId, frndShareId, picName);
	return;
}

bool
MessageProcessor::picDone(int fd)
{
	for (;;)
	{
		auto pMsg{pMsgEnq->getNextMsg(fd)};
		if (!pMsg)
			break;
		int nMsgTyp = pMsg->getMsgTyp();
		if (nMsgTyp < 0)
			continue;
		if (m_pPicSndr->shouldEnqueMsg(pMsg->getFd()))
		{
			pMsgEnq->enqMsg(pMsg);	
			break;
		}
		processMsg(pMsg, nMsgTyp);						
		
	}
	return true;
}

bool
MessageProcessor::sendMsg(char *buf, int mlen, int fd)
{

	if (!pNtwIntf->sendMsg(buf, mlen, fd))
	{
		std::cout << "Failed to send message for fd=" << fd << std::endl;
		return false;
	}
	return true;
}

bool
MessageProcessor::sendMsg(char *buf, int mlen, int fd, bool *tryAgain)
{

	if (!pNtwIntf->sendMsg(buf, mlen, fd, tryAgain))
	{
		//std::cout << "Failed to send message for fd=" << fd << std::endl;
		return false;
	}
	return true;
}


/*
bool
MessageProcessor::sendApplePush(const std::vector<std::string>& tokens, const std::string& msg, int badge)
{

	return pAppleNotfy->send(tokens, msg, badge);
}
*/

bool
MessageProcessor::getShareIds(const std::string& lst, std::vector<std::string>& shareIds)
{

	return m_pTrnsl->getShareIds(lst, shareIds);
}

/*
bool
MessageProcessor::sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg)
{
	std::string fbmsg;
	switch(appId)
	{
		case OPENHOUSES_ID:
			fbmsg = "OpenHouses- ";
		break;

		case AUTOSPREE_ID:
			fbmsg = "AutoSpree- ";
		break;

		case EASYGROCLIST_ID:
			fbmsg = "EasyGrocList- ";
		break;

		default:
		break;
	}
	fbmsg += msg; 
	return pFirebaseNotify->send(tokens, fbmsg);
}
*/

bool
MessageProcessor::getReply(char *buf, int *mlen, int msgTyp)
{
	return m_pTrnsl->getReply(buf, mlen, msgTyp);
}
