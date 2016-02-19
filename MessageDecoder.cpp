#include <MessageDecoder.h>
#include <string.h>
#include <iostream>
#include <functional>

using namespace std::placeholders;

MessageDecoder::MessageDecoder()
{
	for (int i=0; i < NO_COMMON_MSGS; ++i)
		msgTypPrcsrs[i] = -1;
	msgTypPrcsrs[GET_SHARE_ID_MSG] = 0;
	msgTypPrcsrs[STORE_TRNSCTN_ID_MSG] = 1;
	msgTypPrcsrs[STORE_FRIEND_LIST_MSG] = 2;
}

MessageDecoder::~MessageDecoder()
{

}



bool 
MessageDecoder::operator()(char* buffer, ssize_t mlen, int fd)
{
    bool bRet = true;
    int msgTyp;
    memcpy(&msgTyp, buffer+sizeof(int), sizeof(int));
    static auto processors = {std::bind(std::mem_fn(&MessageDecoder::createShareIdObj), this, _1, _2, _3), std::bind(std::mem_fn(&MessageDecoder::createStoreIdObj), this, _1, _2, _3),
std::bind(std::mem_fn(&MessageDecoder::createFrndLstObj), this, _1, _2, _3)}; 
	if (msgTyp > NO_COMMON_MSGS)
		return decodeMsg(buffer, mlen, fd);
	int pindx = msgTypPrcsrs[msgTyp];
	if (pindx == -1)
		return false;
	auto itr = processors.begin();
	return itr[pindx](buffer, mlen, fd);
    
}

bool
MessageDecoder::createShareIdObj(char *buffer,  ssize_t mlen, int fd)
{
      std::unique_ptr<ShareIdObj> pMsg = std::unique_ptr<ShareIdObj>{new ShareIdObj()};	
      pMsg->setMsgTyp(GET_SHARE_ID_MSG);
	constexpr int offset = 2*sizeof(int);
	long tid;
	memcpy(&tid, buffer+offset, sizeof(long));
	pMsg->setTrnId(tid);
	pMsg->setFd(fd);
	constexpr int shidOffset = offset + sizeof(long);
	pMsgs.push_back(std::move(pMsg));
    return true;
}

bool
MessageDecoder::createStoreIdObj(char *buffer,  ssize_t mlen, int fd)
{
      std::unique_ptr<ShareIdObj> pMsg{new ShareIdObj()};	
      pMsg->setMsgTyp(STORE_TRNSCTN_ID_MSG);
	constexpr int offset = 2*sizeof(int);
	long tid;
	memcpy(&tid, buffer+offset, sizeof(long));
	pMsg->setTrnId(tid);
	pMsg->setFd(fd);
	constexpr int shidOffset = offset + sizeof(long);
	long shrId ;
	memcpy (&shrId, buffer + shidOffset, sizeof(long));	
	pMsg->setShrId(shrId);
	pMsgs.push_back(std::move(pMsg));
    return true;
}
std::unique_ptr<MsgObj>
MessageDecoder::getNextMsg()
{
	if (!pMsgs.empty())
	{
		std::unique_ptr<MsgObj> pMsg = std::move(pMsgs.front());
		pMsgs.pop_front();
		return pMsg;
	}
	std::unique_ptr<MsgObj> pNll{};
	return pNll;
}

bool
MessageDecoder::createFrndLstObj(char *buffer, ssize_t mlen, int fd)
{
	std::unique_ptr<FrndLstObj> pMsg = std::unique_ptr<FrndLstObj>{new FrndLstObj()};
	pMsg->setMsgTyp(STORE_FRIEND_LIST_MSG);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setFd(fd);
	constexpr int frndLstOffset = offset + sizeof(long);
	pMsg->setFrndLst(buffer + frndLstOffset);
	pMsgs.push_back(std::move(pMsg));
	return true;
}

void
MessageDecoder::addMsgObj(std::unique_ptr<MsgObj> pMsg)
{
	pMsgs.push_back(std::move(pMsg));
}
