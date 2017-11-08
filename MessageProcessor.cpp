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

using namespace std::placeholders;

MessageProcessor::MessageProcessor():m_pDcd(NULL), m_pTrnsl(NULL), m_pPicSndr(NULL),  pNtwIntf(new NtwIntf<MessageDecoder>()), pArch(new ArchiveSndr()), pMsgEnq(new MessageEnqueuer()), dataStore{CommonDataMgr::Instance()}
{
	std::cout << "Setting MessageProcessor message to member function mapping " << __FILE__ << ":" << __LINE__ << std::endl;
	for (auto &msgTypPrc : msgTypPrcsrs)
		msgTypPrc = -1;
	msgTypPrcsrs[GET_SHARE_ID_MSG] = 0;
	msgTypPrcsrs[STORE_TRNSCTN_ID_MSG] = 1;
	msgTypPrcsrs[STORE_FRIEND_LIST_MSG] = 2;
	msgTypPrcsrs[ARCHIVE_ITEM_MSG] = 3;
	msgTypPrcsrs[SHARE_ITEM_MSG] = 4;
	msgTypPrcsrs[STORE_DEVICE_TKN_MSG] = 5;
	msgTypPrcsrs[GET_ITEMS] = 6;
	msgTypPrcsrs[PIC_METADATA_MSG] = 7;
	msgTypPrcsrs[PIC_MSG] = 8;
	msgTypPrcsrs[PIC_DONE_MSG] = 9;
	msgTypPrcsrs[SHOULD_DOWNLOAD_MSG] = 10;


	
}

MessageProcessor::~MessageProcessor()
{

}

bool
MessageProcessor::process()
{
	for (;;)
	{
        	pFirebaseNotify->getSendEvents();
		if (!pNtwIntf->waitAndGetMsg())	
		{
			--nFds;
		}
		else
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
		std::cout << "Exceeded max capacity in worker fds" << std::endl;
		close(fd);
		return false;
	}
	if (pNtwIntf->addFd(fd))
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
	return;
}

void
MessageProcessor::onCloseFd(int fd)
{
	std::vector<PicFileDetails> pfdVec = m_pPicSndr->onCloseNtwFd(fd);
	for(const auto& pfd : pfdVec)
	{
		std::cout << "Updating picSharestatus for " << pfd.appId << std::endl;
	}
}


void
MessageProcessor::processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg, int nMsgTyp)
{
	static auto processors = {std::bind(std::mem_fn(&MessageProcessor::processShareIdMsg), this, _1)
	,std::bind(std::mem_fn(&MessageProcessor::processStoreIdMsg), this, _1)
	,std::bind(std::mem_fn(&MessageProcessor::processFrndLstMsg), this, _1)
       ,std::bind(std::mem_fn(&MessageProcessor::processArchvItemMsg), this, _1)
	,std::bind(std::mem_fn(&MessageProcessor::processItemMsg), this, _1) 
	,std::bind(std::mem_fn(&MessageProcessor::processDeviceTknMsg), this, _1)
	, std::bind(std::mem_fn(&MessageProcessor::processGetItemMsg), this, _1)
	, std::bind(std::mem_fn(&MessageProcessor::processPicMetaDataMsg), this, _1)
	, std::bind(std::mem_fn(&MessageProcessor::processPicMsg), this, _1)
	, std::bind(std::mem_fn(&MessageProcessor::processPicDoneMsg), this, _1)
	, std::bind(std::mem_fn(&MessageProcessor::processShouldDownLoadMsg), this, _1)
};
	auto itr = processors.begin();
    if (nMsgTyp >= NO_COMMON_MSGS)
        return;
    int pindx =msgTypPrcsrs[nMsgTyp];
    
    if (pindx == -1)
    {
        std::cout << "No handler found for msgTyp=" << nMsgTyp << std::endl;
        return;
    }

	itr[pindx](pMsg);

	return;
}

void
MessageProcessor::processRequests()
{
		for (;;)
	{
		std::unique_ptr<MsgObj, MsgObjDeltr> pMsg{m_pDcd->getNextMsg()};
		if (!pMsg)
			break;
		int nMsgTyp = pMsg->getMsgTyp();
		if (nMsgTyp < 0)
			continue;
		if (nMsgTyp > NO_COMMON_MSGS)
		{
			processMsg(pMsg);
			continue;
		}	
		if (msgTypPrcsrs[nMsgTyp] == -1)
			continue;
		if (nMsgTyp != SHOULD_DOWNLOAD_MSG && m_pPicSndr->shouldEnqueMsg(pMsg->getFd()))
		{
			pMsgEnq->enqMsg(std::move(pMsg));	
			continue;
		}
		processMsg(pMsg, nMsgTyp);						
	}
	return;
}

    
    void
    MessageProcessor::processShouldDownLoadMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
    {
        ShouldDownLoad  *pShouldDownLoad  = dynamic_cast<ShouldDownLoad*>(pMsg.get());
        if (!pShouldDownLoad )
        {
            std::cout << "Invalid message received in MessageProcessor::processShouldDownLoadMsg " << " " << __FILE__ << ":" << __LINE__ << std::endl;	
            return;
        }
	std::cout << "Received ShouldDownLoad message " << *pShouldDownLoad << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	m_pPicSndr->updateWaitingInfo(pShouldDownLoad->getName(), pShouldDownLoad->getShrId(), pShouldDownLoad->downLoad());
    }

void
MessageProcessor::processPicMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	PicObj *pPicObj = dynamic_cast<PicObj*>(pMsg.get());
	if (!pPicObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicMsg " << std::endl;
		return;
	}
	std::cout << "Storing picture object " << __FILE__ << ":" << __LINE__ << std::endl;
	if (dataStore.storePic(pPicObj))
	{
		std::cout << "Stored picture object " << __FILE__ << ":" << __LINE__ << std::endl;
		//send push notification
		std::vector<std::string> shareIds = dataStore.getPicShareIds(pPicObj->getFd()); 
		if (shareIds.size())
		{
			
			std::string picName = dataStore.getPicName(pPicObj->getFd());
			std::cout << "Sending push notification to receive picture name=" << picName << " " << __FILE__ << ":" << __LINE__ << std::endl;
			dataStore.eraseFdMp(pPicObj->getFd());
			sendPicNotifications(shareIds, pPicObj->getAppId(), picName);
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
		sendApplePush(tokens, picName, 1);
	}
	std::vector<std::string> regIds;
	dataStore.getAndroidDeviceTkns(appId, shareIds, regIds);
	if (regIds.size())
	{
		sendFirebaseMsg(appId, regIds, picName);
	}
}

void
MessageProcessor::processPicMetaDataMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{

	PicMetaDataObj *pPicMetaObj = dynamic_cast<PicMetaDataObj*>(pMsg.get());
	if (!pPicMetaObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicMetaDataMsg " << std::endl;
		return;
	}
	int picOffset = 0;
	bool bShoulUpload = dataStore.storePicMetaData(pPicMetaObj, &picOffset);
	const std::vector<std::string>&  shareIds = pPicMetaObj->getFrndLst();
	for (const std::string& shareId : shareIds)
	{
		char archbuf[32768];
		int archlen = 0;
		std::cout << "Archiving shareId=" << shareId << " in processPicMetaDataMsg " << __FILE__ << ":" << __LINE__ << std::endl;
		if (ArchiveMsgCreator::createPicMetaDataMsg(archbuf, archlen, pPicMetaObj->getAppId(), false, std::stol(shareId), pPicMetaObj->getShrId(), pPicMetaObj->getName(),  32768, pPicMetaObj->getPicLen()))
			sendArchiveMsg(archbuf, archlen, 10);	
	}
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
MessageProcessor::processPicDoneMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	PicDoneObj *pPicDoneObj = dynamic_cast<PicDoneObj*>(pMsg.get());
	if (!pPicDoneObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicDoneMsg " << std::endl;
		return;
	}
	std::cout << "Received PicDone Msg=" << *pPicDoneObj << " " << __FILE__ << ":" << __LINE__ << std::endl;
	updatePicShareInfo(pPicDoneObj->getAppId(), pPicDoneObj->getShrId(), pPicDoneObj->getPicShareId(), pPicDoneObj->getPicName());
	char archbuf[32768];
	int archlen = 0;
	std::cout << "Updating Archive with picDoneMsg in processPicDoneMsg " << __FILE__ << ":" << __LINE__ << std::endl;
	if (ArchiveMsgCreator::createPicMetaDataMsg(archbuf, archlen, pPicDoneObj->getAppId(), true, pPicDoneObj->getShrId(), pPicDoneObj->getPicShareId(), pPicDoneObj->getPicName(),  32768, 100))
		sendArchiveMsg(archbuf, archlen, 10);	


}

void
MessageProcessor::processGetItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	GetItemObj *pGetItemObj = dynamic_cast<GetItemObj*>(pMsg.get());
	if (!pGetItemObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processGetItemMsg " << std::endl;
		return;
	}

	std::cout << "Received GetItem Msg=" << *pGetItemObj << " " << __FILE__ << ":" << __LINE__ << std::endl;

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
				dataStore.updateLstShareInfo(pGetItemObj->getAppId(), pGetItemObj->getShrId(), pItr->first.shareId, pItr->first.lstName);
				if (ArchiveMsgCreator::createShareLstMsg(archbuf, archlen, pGetItemObj->getAppId(), true, pGetItemObj->getShrId(), pItr->first.shareId, pItr->first.lstName, 32768))
					sendArchiveMsg(archbuf, archlen, 10);	
			}
		}
	}
    
    std::map<shrIdLstName, std::string> templLstNameMp;
    dataStore.getShareTemplLists(pGetItemObj->getAppId(), pGetItemObj->getShrId(), templLstNameMp);
    
    archlen = 0;
    
    for(auto pItr = templLstNameMp.begin(); pItr != templLstNameMp.end(); ++pItr)
    {
        std::string itemName = std::to_string(pItr->first.shareId);
        itemName += ":::";
        itemName += pItr->first.lstName;
        if (m_pTrnsl->getListMsg(archbuf, &archlen, 32768, pItr->first.lstName, pItr->second, SHARE_TEMPL_ITEM_MSG, pItr->first.shareId))
        {
            if (sendMsg(archbuf, archlen, pGetItemObj->getFd()))
            {
                dataStore.updateTemplLstShareInfo(pGetItemObj->getAppId(), pGetItemObj->getShrId(), pItr->first.shareId, pItr->first.lstName);
                if (ArchiveMsgCreator::createShareTemplLstMsg(archbuf, archlen, pGetItemObj->getAppId(), true, pGetItemObj->getShrId(), pItr->first.shareId, pItr->first.lstName, 32768))
                    sendArchiveMsg(archbuf, archlen, 10);
            }
        }
    }
    processGetItemPics(pGetItemObj);
    return;
}
    
void
MessageProcessor::processGetItemPics(GetItemObj *pGetItemObj)
{
    std::vector<shrIdLstName> picNamesShIds;
    dataStore.getPictureNames(pGetItemObj->getAppId(), pGetItemObj->getShrId(), picNamesShIds);
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
        m_pPicSndr->insertPicNameShid(picNameShId);
    }
    
}

void
MessageProcessor::processItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	LstObj *pLstObj = dynamic_cast<LstObj*>(pMsg.get());
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
			sendApplePush(tokens, pLstObj->getName(), 1);
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
MessageProcessor::processArchvItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	TemplLstObj *pTmplObj = dynamic_cast<TemplLstObj*>(pMsg.get());
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
MessageProcessor::processDeviceTknMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	DeviceTknObj *pDevTknObj = dynamic_cast<DeviceTknObj*>(pMsg.get());
	if (!pDevTknObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processDeviceTknMsg " << std::endl;
		return;
	}
	dataStore.storeDeviceTkn(pDevTknObj->getAppId(), pDevTknObj->getShrId(), pDevTknObj->getDeviceTkn(), pDevTknObj->getPlatform());
	std::unique_ptr<char> pArchMsg;
	char archbuf[8192];
	int archlen = 0;
	if (ArchiveMsgCreator::createDevTknMsg(archbuf, archlen, pDevTknObj->getAppId(),  pDevTknObj->getShrId(), pDevTknObj->getDeviceTkn(), pDevTknObj->getPlatform()))
		sendArchiveMsg(archbuf, archlen, 10);	
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
MessageProcessor::processShareIdMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	ShareIdObj *pShObj = dynamic_cast<ShareIdObj*>(pMsg.get());

	long shareId = 0;
	bool archive;
	if (pShObj)
		shareId = ShareIdMgr::Instance().getShareId(pShObj->getTrnId(), archive);
	if (shareId)
	{
		if (archive)
		{
			//send message to archiver
			char archbuf[256];
			int archlen=0;
			if (ArchiveMsgCreator::createShareIdMsg(archbuf, archlen, shareId))
				pArch->sendMsg(archbuf, archlen, 10);
		}
		//send reply
		char buf[512];
		int mlen=0;
		if (m_pTrnsl->getShareIdReply(buf, &mlen, shareId))
		{
			if (!pNtwIntf->sendMsg(buf, mlen, pShObj->getFd()))
				std::cout << "Failed to send message for fd=" << pShObj->getFd() << std::endl;
		}
		std::cout << "Replying with shareId=" << shareId << " " << __FILE__ << " " << __LINE__ << std::endl;
	}
	return;

}

void
MessageProcessor::processStoreIdMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	ShareIdObj *pShObj = dynamic_cast<ShareIdObj*>(pMsg.get());
	if (!pShObj)
		return;
	bool archive;
	ShareIdMgr::Instance().storeTrndId(pShObj->getTrnId(), pShObj->getShrId(), archive);
	shrIdTrnId shtrId{pShObj->getShrId(), pShObj->getTrnId()};

	if (archive)
	{
		char archbuf[256];
		int archlen=0;
		if (ArchiveMsgCreator::createTrnIdShrIdMsg(archbuf, archlen, shtrId))
			pArch->sendMsg(archbuf, archlen, 10);
	}
	char buf[512];
	int mlen =0;
	if (m_pTrnsl->getStoreIdReply(buf, &mlen))
	{
		if (!pNtwIntf->sendMsg(buf, mlen, pShObj->getFd()))
			std::cout << "Failed to send message for fd=" << pShObj->getFd() << std::endl;
	}

	return;
}

void
MessageProcessor::processFrndLstMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	FrndLstObj *pFrndObj = dynamic_cast<FrndLstObj*>(pMsg.get());
	if (!pFrndObj)
		return;
	std::cout << "Received friend list message shareId=" << pFrndObj->getShrId() << " friend list=" << pFrndObj->getFrndLst() <<  " " << __FILE__ << ":" << __LINE__  << std::endl;
	FrndLstMgr::Instance().storeFrndLst(pFrndObj);
	std::unique_ptr<char> pArchMsg;
	char archbuf[8192];
	int archlen =0;
	pArchMsg =  ArchiveMsgCreator::createFrndLstMsg(archbuf, archlen, pFrndObj->getShrId(), pFrndObj->getFrndLst(), 8192);
	pArch->sendMsg(pArchMsg == nullptr ? archbuf : pArchMsg.get(), archlen, 10);
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
MessageProcessor::notify(char *buf, int mlen, int fd)
{
	return sendMsg(buf, mlen , fd);
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
		std::unique_ptr<MsgObj, MsgObjDeltr> pMsg{pMsgEnq->getNextMsg(fd)};
		if (!pMsg)
			break;
		int nMsgTyp = pMsg->getMsgTyp();
		if (nMsgTyp < 0)
			continue;
		if (msgTypPrcsrs[nMsgTyp] == -1)
			continue;
		if (m_pPicSndr->shouldEnqueMsg(pMsg->getFd()))
		{
			pMsgEnq->enqMsg(std::move(pMsg));	
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

void
MessageProcessor::setAppleNotify(std::shared_ptr<ApplePush> pAppleNtfy)
{
	pAppleNotfy = pAppleNtfy;
	return;
}

void
MessageProcessor::setFirebaseNotify(std::shared_ptr<FirebaseConnHdlr> pFirebaseNtfy)
{
    pFirebaseNotify = pFirebaseNtfy;
}

bool
MessageProcessor::sendApplePush(const std::vector<std::string>& tokens, const std::string& msg, int badge)
{

	return pAppleNotfy->send(tokens, msg, badge);
}

bool
MessageProcessor::getShareIds(const std::string& lst, std::vector<std::string>& shareIds)
{

	return m_pTrnsl->getShareIds(lst, shareIds);
}

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

bool
MessageProcessor::getReply(char *buf, int *mlen, int msgTyp)
{
	return m_pTrnsl->getReply(buf, mlen, msgTyp);
}
