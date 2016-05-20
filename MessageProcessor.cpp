#include <MessageProcessor.h>
#include <ShareIdMgr.h>
#include <ArchiveMsgCreator.h>
#include <MessageObjs.h>
#include <Constants.h>
#include <iostream>
#include <sstream>
#include <functional>
#include <FrndLstMgr.h>

using namespace std::placeholders;

MessageProcessor::MessageProcessor():m_pDcd(NULL), m_pTrnsl(NULL), m_pPicSndr(NULL),  pNtwIntf(new NtwIntf<MessageDecoder>()), pArch(new ArchiveSndr()), pMsgEnq(new MessageEnqueuer()), dataStore{CommonDataMgr::Instance()}
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
	pNtwIntf->setPicSndr(m_pPicSndr);
	m_pPicSndr->setTrnsl(pTrnsl);
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
		std::unique_ptr<MsgObj, MsgObjDeltr> pMsg{m_pDcd->getNextMsg()};
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
MessageProcessor::processPicMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	PicObj *pPicObj = dynamic_cast<PicObj*>(pMsg.get());
	if (!pPicObj)
	{
		std::cout << "Invalid message received in MessageProcessor::processPicMsg " << std::endl;
		return;
	}
	if (dataStore.storePic(pPicObj))
	{
		//send push notification
		std::vector<std::string> shareIds = dataStore.getPicShareIds(pPicObj->getFd()); 
		if (shareIds.size())
		{
			std::vector<std::string> tokens;
			dataStore.getDeviceTkns(pPicObj->getAppId(), shareIds, tokens);	
			std::string picName = dataStore.getPicName(pPicObj->getFd());
			sendApplePush(tokens, picName, 1);
			dataStore.eraseFdMp(pPicObj->getFd());
		}
	
	}
	return;
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
	dataStore.storePicMetaData(pPicMetaObj);
	return;
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
	std::map<shrIdLstName, std::string> lstNameMp;
	dataStore.getShareLists(pGetItemObj->getAppId(), pGetItemObj->getShrId(), lstNameMp);
	char archbuf[32768];
	int archlen = 0;
	for (auto pItr = lstNameMp.begin(); pItr != lstNameMp.end(); ++pItr)
	{
		std::string itemName = std::to_string(pItr->first.shareId);
		itemName += ":::";
		itemName += pItr->first.lstName;
		if (m_pTrnsl->getListMsg(archbuf, &archlen, 32768, itemName, pItr->second))
		{
			if (sendMsg(archbuf, archlen, pGetItemObj->getFd()))
			{
				dataStore.updateLstShareInfo(pGetItemObj->getAppId(), pGetItemObj->getShrId(), pItr->first.shareId, pItr->first.lstName);
				if (ArchiveMsgCreator::createShareLstMsg(archbuf, archlen, pGetItemObj->getAppId(), true, pGetItemObj->getShrId(), pItr->first.shareId, pItr->first.lstName, 32768))
					sendArchiveMsg(archbuf, archlen, 10);	
			}
		}
	}	
	std::vector<shrIdLstName> picNamesShIds;
	dataStore.getPictureNames(pGetItemObj->getAppId(), pGetItemObj->getShrId(), picNamesShIds);
	for (auto& picNameShId : picNamesShIds)
	{
		picNameShId.fd = pGetItemObj->getFd();
		m_pPicSndr->insertPicNameShid(picNameShId);
	}
	return;
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
	dataStore.storeItem(pLstObj->getAppId(), pLstObj->getShrId(), pLstObj->getName(), pLstObj->getList());	
	
	//int size= 

	char archbuf[32768];
	int archlen = 0;
	if (ArchiveMsgCreator::createItemMsg(archbuf, archlen, pLstObj->getAppId(), pLstObj->getShrId(), pLstObj->getName(), pLstObj->getList(), 32768))
		sendArchiveMsg(archbuf , archlen, 10);	

	std::vector<std::string>  shareIds;
	if (m_pTrnsl->getShareIds(pLstObj->getList(), shareIds))
	{
		dataStore.storeLstShareInfo(pLstObj->getAppId(),pLstObj->getShrId(), shareIds, pLstObj->getName());
		for (const std::string& shareId : shareIds)
		{
			if (ArchiveMsgCreator::createShareLstMsg(archbuf, archlen, pLstObj->getAppId(), false, std::stol(shareId), pLstObj->getShrId(), pLstObj->getName(),  32768))
				sendArchiveMsg(archbuf, archlen, 10);	
		}
		std::vector<std::string> tokens;
		dataStore.getDeviceTkns(pLstObj->getAppId(), shareIds, tokens);	
		sendApplePush(tokens, pLstObj->getName(), 1);
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
	dataStore.storeDeviceTkn(pDevTknObj->getAppId(), pDevTknObj->getShrId(), pDevTknObj->getDeviceTkn());
	std::unique_ptr<char> pArchMsg;
	char archbuf[8192];
	int archlen = 0;
	if (ArchiveMsgCreator::createDevTknMsg(archbuf, archlen, pDevTknObj->getAppId(),  pDevTknObj->getShrId(), pDevTknObj->getDeviceTkn()))
		sendArchiveMsg(archbuf, archlen, 10);	
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, STORE_DEVICE_TKN_RPLY_MSG))
	{
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
