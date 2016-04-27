#include <MessageProcessor.h>
#include <ShareIdMgr.h>
#include <ArchiveMsgCreator.h>
#include <MessageObjs.h>
#include <Constants.h>
#include <iostream>
#include <functional>
#include <FrndLstMgr.h>

using namespace std::placeholders;

MessageProcessor::MessageProcessor():m_pDcd(NULL), m_pTrnsl(NULL), pNtwIntf(new NtwIntf<MessageDecoder>()), pArch(new ArchiveSndr())
{
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


	
}

MessageProcessor::~MessageProcessor()
{

}

bool
MessageProcessor::process()
{
	for (;;)
	{
		if (!pNtwIntf->waitAndGetMsg())
			--nFds;
		else
		{
			processRequests();
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
	m_pTrnsl = pTrnsl;
	pNtwIntf->setDecoder(m_pDcd);
	return;
}

void
MessageProcessor::processRequests()
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
};
	auto itr = processors.begin();
	for (;;)
	{
		std::unique_ptr<MsgObj> pMsg{m_pDcd->getNextMsg()};
		if (!pMsg)
			continue;
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
		itr[msgTypPrcsrs[nMsgTyp]](pMsg);
						
	}
	return;
}

void
MessageProcessor::processPicMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	PicObj *pPicObj = dynamic_cast<PicObj*>(pMsg.get());
	if (!pPicObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicMsg " << std::endl;
		return;
	}
	return;
}

void
MessageProcessor::processPicMetaDataMsg(const std::unique_ptr<MsgObj>& pMsg)
{

	PicMetaDataObj *pPicMetaObj = dynamic_cast<PicMetaDataObj*>(pMsg.get());
	if (!pPicMetaObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicMetaDataMsg " << std::endl;
		return;
	}
	dataStore.storePicMetaData(pPicMetaObj->getAppId(), pPicMetaObj->getShrId(), pPicMetaObj->getName(), pPicMetaObj->getFrndLst());
	fdPicMetaMp[pPicMetaObj->getFd()] = std::move(pMsg);
	return;
}


void
MessageProcessor::processGetItemMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	GetItemObj *pGetItemObj = dynamic_cast<GetItemObj*>(pMsg.get());
	if (!pGetItemObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processGetItemMsg " << std::endl;
		return;
	}
	std::map<std::string, std::string> lstNameMp;
	dataStore.getShareLists(pGetItemObj->getAppId(), pGetItemObj->getShrId(), pGetItemObj->getDeviceId(), lstNameMp);
	std::unique_ptr<char> pArchMsg;
	char archbuf[32768];
	int archlen = 0;
	for (auto pItr = lstNameMp.begin(); pItr != lstNameMp.end(); ++pItr)
	{
		pArchMsg = m_pTransl->getListMsg(archbuf, &archlen, 32768, pItr->first, pItr->second);	
		if (sendMsg(pArchMsg == nullptr? archbuf:pArchMsg.get(), archlen, pGetItemObj->getFd()))
		{
			std::string val = dataStore.updateLstShareInfo(pGetItemObj->getAppId(), pGetItemObj->getShrId(), pGetItemObj->getDeviceId(), pItr->first);
			if (ArchiveMsgCreator::createShareLstMsg(archbuf, archlen, pGetItemObj->getShrId(), pItr->first, val, 32768))
				sendArchiveMsg(archbuf, archlen, 10);	
		}
	}	
	return;
}

void
MessageProcessor::processItemMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	LstObj *pLstObj = dynamic_cast<LstObj*>(pMsg.get());
	if (!pLstObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processItemMsg " << std::endl;
		return;
	}
	dataStore.storeItem(pLstObj->getAppId(), pLstObj->getShrId(), pLstObj->getName(), pLstObj->getList());	
	
	//int size= 

	std::unique_ptr<char> pArchMsg;
	char archbuf[32768];
	int archlen = 0;
	pArchMsg = ArchiveMsgCreator::createItemMsg(archbuf, archlen, pLstObj->getShrId(), pLstObj->getName(), pLstObj->getList(), 32768);
	sendArchiveMsg(pArchMsg == nullptr?archbuf : pArchMsg.get(), archlen, 10);	

	std::vector<std::string>  shareIds;
	if (m_pTransl->getShareIds(pLstObj->getList(), shareIds))
	{
		dataStore.storeLstShareInfo(pLstObj->getAppId(), shareIds, pLstObj->getName());
		struct timeval tv;
		gettimeofday(&tv, NULL);
		std::ostringstream valstream;
		valstream  << tv.tv_sec << ";";
		for (const std::string& shareId : shareIds)
		{
			if (ArchiveMsgCreator::createShareLstMsg(archbuf, archlen, std::stol(shareId), pLstObj->getName(), valstream.str(), 32768))
				sendArchiveMsg(archbuf, archlen, 10);	
		}
		std::vector<std::string> tokens;
		dataStore.getDeviceTkns(pLstObj->getAppId(), shareIds, tokens);	
		sendApplePush(tokens, pLstObj->getName(), 1);
	}
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, SHARE_LIST_RPLY_MSG))
	{
		sendMsg(buf, mlen, pLstObj->getFd());
	}

	
	return;
}

void
MessageProcessor::processArchvItemMsg(const std::unique_ptr<MsgObj>& pMsg)
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
	if (m_pTrnsl->getReply(buf, &mlen, STORE_TEMPL_LIST_RPLY_MSG))
	{
		sendMsg(buf, mlen, pTmplObj->getFd());
	}
	return;
}

void
MessageProcessor::processDeviceTknMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	DeviceTknObj *pDevTknObj = dynamic_cast<DeviceTknObj*>(pMsg.get());
	if (!pDevTknObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processDeviceTknMsg " << std::endl;
		return;
	}
	dataStore.storeDeviceTkn(pDevTknObj->getAppId(), pDevTknObj->getShrId(), pDevTknObj->getDeviceId(), pDevTknObj->getDeviceTkn());
	std::unique_ptr<char> pArchMsg;
	char archbuf[8192];
	int archlen = 0;
	if (ArchiveMsgCreator::createDevTknMsg(archbuf, archlen, pDevTknObj->getShrId(), pDevTknObj->getDeviceId(), pDevTknObj->getDeviceTkn()))
		sendArchiveMsg(archbuf, archlen, 10);	
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, STORE_EASYGROC_DEVICE_TKN_RPLY_MSG))
	{
		sendMsg(buf, mlen, pDevTknObj->getFd());
	}
		
	return;
}



void
MessageProcessor::processShareIdMsg(const std::unique_ptr<MsgObj>& pMsg)
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
			shrIdTrnId shtrId{shareId, pShObj->getTrnId()};
			if (ArchiveMsgCreator::createTrnIdShrIdMsg(archbuf, archlen, shtrId))
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
	}
	return;

}

void
MessageProcessor::processStoreIdMsg(const std::unique_ptr<MsgObj>& pMsg)
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
MessageProcessor::processFrndLstMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	FrndLstObj *pFrndObj = dynamic_cast<FrndLstObj*>(pMsg.get());
	if (!pFrndObj)
		return;
	FrndLstMgr::Instance().storeFrndLst(pFrndObj);
	int size = sizeof(long) + pFrndObj->getFrndLst().size() + 1;
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

bool
MessageProcessor::sendApplePush(const std::vector<std::string>& tokens, const std::string& msg, int badge)
{

	return pAppleNotfy->send(tokens, msg, badge);
}
