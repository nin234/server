#include <EasyGrocMsgProcessor.h>
#include <EasyGrocDecoder.h>
#include <EasyGrocTranslator.h>
#include <functional>
#include <iostream>
#include <MessageObjs.h>
#include <EasyGrocDataMgr.h>
#include <ArchiveMsgCreator.h>
#include <vector>
#include <sstream>

using namespace std::placeholders;

EasyGrocMsgProcessor::EasyGrocMsgProcessor():dataStore{EasyGrocDataMgr::Instance()}
{
	MessageProcessor::setDcdTransl(new EasyGrocDecoder(), new EasyGrocTranslator());	
	for (auto &msgTypPrc : msgTypPrcsrs)
		msgTypPrc = -1;
	msgTypPrcsrs[STORE_TEMPL_LIST_MSG%NO_EASYGROC_MSGS] = 0;
	msgTypPrcsrs[SHARE_LIST_MSG%NO_EASYGROC_MSGS] = 1;
	msgTypPrcsrs[STORE_EASYGROC_DEVICE_TKN_MSG%NO_EASYGROC_MSGS] = 2;
	msgTypPrcsrs[GET_EASYGROC_ITEMS%NO_EASYGROC_MSGS] = 2;


}

EasyGrocMsgProcessor::~EasyGrocMsgProcessor()
{

}

void
EasyGrocMsgProcessor::processMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	static auto processors = {std::bind(std::mem_fn(&EasyGrocMsgProcessor::processTemplLstMsg), this, _1), std::bind(std::mem_fn(&EasyGrocMsgProcessor::processLstMsg), this, _1), 
	std::bind(std::mem_fn(&EasyGrocMsgProcessor::processDeviceTknMsg), this, _1), std::bind(std::mem_fn(&EasyGrocMsgProcessor::processGetItemMsg), this, _1)};
	int msgTyp = pMsg->getMsgTyp();
	
	if (msgTyp < EASY_GROC_MSG_START || msgTyp > EASY_GROC_MSG_END)
	{
		std::cout << "Invalid message type " << msgTyp << "received in EasyGrocMsgProcessor " << std::endl;
		return;
	}
	int pindx = msgTypPrcsrs[msgTyp%NO_EASYGROC_MSGS];
	if (pindx == -1)
		return;
	auto itr = processors.begin();
	itr[pindx](pMsg);
	
	return;
}

void
EasyGrocMsgProcessor::processGetItemMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	GetItemObj *pGetItemObj = dynamic_cast<GetItemObj*>(pMsg.get());
	if (!pGetItemObj)
	{
		std::cout << "Invalid message received in EasyGrocMsgProcessor::processGetItemMsg " << std::endl;
		return;
	}
	EasyGrocTranslator *m_pTranslEasy = dynamic_cast<EasyGrocTranslator*>(m_pTrnsl);
	if (!m_pTranslEasy)
		return;
	std::map<std::string, std::string> lstNameMp;
	dataStore.getShareLists(pGetItemObj->getShrId(), pGetItemObj->getDeviceId(), lstNameMp);
	std::unique_ptr<char> pArchMsg;
	char archbuf[32768];
	int archlen = 0;
	for (auto pItr = lstNameMp.begin(); pItr != lstNameMp.end(); ++pItr)
	{
		pArchMsg = m_pTranslEasy->getListMsg(archbuf, &archlen, 32768, pItr->first, pItr->second);	
		if (sendMsg(pArchMsg == nullptr? archbuf:pArchMsg.get(), archlen, pGetItemObj->getFd()))
		{
			std::string val = dataStore.updateLstShareInfo(pGetItemObj->getShrId(), pGetItemObj->getDeviceId(), pItr->first);
			if (ArchiveMsgCreator::createShareLstMsg(archbuf, archlen, pGetItemObj->getShrId(), pItr->first, val, 32768))
				sendArchiveMsg(archbuf, archlen, 10);	
		}
	}	
	return;
}


void
EasyGrocMsgProcessor::processLstMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	EasyGrocTranslator *m_pTranslEasy = dynamic_cast<EasyGrocTranslator*>(m_pTrnsl);
	if (!m_pTranslEasy)
		return;
	LstObj *pLstObj = dynamic_cast<LstObj*>(pMsg.get());
	if (!pLstObj)
	{
		std::cout << "Invalid message received in EasyGrocMsgProcessor::processLstMsg " << std::endl;
		return;
	}
	dataStore.storeList(pLstObj->getShrId(), pLstObj->getName(), pLstObj->getList());	
	
	//int size= 

	std::unique_ptr<char> pArchMsg;
	char archbuf[32768];
	int archlen = 0;
	pArchMsg = ArchiveMsgCreator::createLstMsg(archbuf, archlen, pLstObj->getShrId(), pLstObj->getName(), pLstObj->getList(), 32768);
	sendArchiveMsg(pArchMsg == nullptr?archbuf : pArchMsg.get(), archlen, 10);	

	std::vector<std::string>  shareIds;
	if (m_pTranslEasy->getShareIds(pLstObj->getList(), shareIds))
	{
		dataStore.storeLstShareInfo(shareIds, pLstObj->getName());
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
		dataStore.getDeviceTkns(shareIds, tokens);	
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
EasyGrocMsgProcessor::processTemplLstMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	TemplLstObj *pTmplObj = dynamic_cast<TemplLstObj*>(pMsg.get());
	if (!pTmplObj)
	{
		std::cout << "Invalid message received in EasyGrocMsgProcessor::processTemplLstMsg " << std::endl;
		return;
	}
	dataStore.storeTemplList(pTmplObj->getShrId(), pTmplObj->getName(), pTmplObj->getTemplList());	
	
	//int size= 

	std::unique_ptr<char> pArchMsg;
	char archbuf[32768];
	int archlen = 0;
	pArchMsg = ArchiveMsgCreator::createTemplLstMsg(archbuf, archlen, pTmplObj->getShrId(), pTmplObj->getName(), pTmplObj->getTemplList(), 32768);
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
EasyGrocMsgProcessor::processDeviceTknMsg(const std::unique_ptr<MsgObj>& pMsg)
{
	DeviceTknObj *pDevTknObj = dynamic_cast<DeviceTknObj*>(pMsg.get());
	if (!pDevTknObj)
	{
		std::cout << "Invalid message received in EasyGrocMsgProcessor::processDeviceTknMsg " << std::endl;
		return;
	}
	dataStore.storeDeviceTkn(pDevTknObj->getShrId(), pDevTknObj->getDeviceId(), pDevTknObj->getDeviceTkn());
	std::unique_ptr<char> pArchMsg;
	char archbuf[8192];
	int archlen = 0;
	if (ArchiveMsgCreator::createEasyGrocDevTknMsg(archbuf, archlen, pDevTknObj->getShrId(), pDevTknObj->getDeviceId(), pDevTknObj->getDeviceTkn()))
		sendArchiveMsg(archbuf, archlen, 10);	
	char buf[1024];
	int mlen=0;
	if (m_pTrnsl->getReply(buf, &mlen, STORE_EASYGROC_DEVICE_TKN_RPLY_MSG))
	{
		sendMsg(buf, mlen, pDevTknObj->getFd());
	}
		
	return;
}


