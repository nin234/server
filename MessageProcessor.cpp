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
	for (int i=0; i < NO_COMMON_MSGS; ++i)
		msgTypPrcsrs[i] = -1;
	msgTypPrcsrs[GET_SHARE_ID_MSG] = 0;
	msgTypPrcsrs[STORE_TRNSCTN_ID_MSG] = 1;
	
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
	static auto processors = {std::bind(std::mem_fn(&MessageProcessor::processShareIdMsg), this, _1), std::bind(std::mem_fn(&MessageProcessor::processStoreIdMsg), this, _1),
	std::bind(std::mem_fn(&MessageProcessor::processFrndLstMsg), this, _1)};
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
