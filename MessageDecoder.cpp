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
	msgTypPrcsrs[ARCHIVE_ITEM_MSG] = 3;
	msgTypPrcsrs[SHARE_ITEM_MSG] = 4;
	msgTypPrcsrs[STORE_DEVICE_TKN_MSG] = 5;
	msgTypPrcsrs[GET_ITEMS] = 6;
	msgTypPrcsrs[PIC_METADATA_MSG] = 7;
	msgTypPrcsrs[PIC_MSG] = 8;
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
    static auto processors = {
    std::bind(std::mem_fn(&MessageDecoder::createShareIdObj), this, _1, _2, _3), 
    std::bind(std::mem_fn(&MessageDecoder::createStoreIdObj), this, _1, _2, _3),
    std::bind(std::mem_fn(&MessageDecoder::createFrndLstObj), this, _1, _2, _3),
    std::bind(std::mem_fn(&MessageDecoder::createTemplLstObj), this, _1, _2, _3), 
    std::bind(std::mem_fn(&MessageDecoder::createLstObj), this, _1, _2, _3),
    std::bind(std::mem_fn(&MessageDecoder::createDeviceTknObj), this, _1, _2, _3),
    std::bind(std::mem_fn(&MessageDecoder::createGetItemObj), this, _1, _2, _3),
    std::bind(std::mem_fn(&MessageDecoder::createPicMetaDataObj), this, _1, _2, _3),
    std::bind(std::mem_fn(&MessageDecoder::createPicObj), this, _1, _2, _3)
}; 
	if (msgTyp > NO_COMMON_MSGS)
		return decodeMsg(buffer, mlen, fd);
	int pindx = msgTypPrcsrs[msgTyp];
	if (pindx == -1)
		return false;
	auto itr = processors.begin();
	return itr[pindx](buffer, mlen, fd);
    
}

bool
MessageDecoder::createPicObj(char *buffer,  ssize_t mlen, int fd)
{
	auto pItr = picObjs.find(fd);
	PicObj *pPic;
	if (pItr == picObjs.end())	
	{
		pPic = new PicObj();
	}
	else
	{
		pPic = pItr->second;
	}
	auto del = [] (PicObj*){};
	std::unique_ptr<PicObj> pMsg(pPic, del);
	pMsg->setMsgTyp(PIC_MSG);	
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	pMsg->setBuf(buffer+2*sizeof(int), mlen-2*sizeof(int));
	int len;
	memcpy(&len, buffer, sizeof(int));
	pMsg->setLen(len);
	pMsgs.push_back(std::move(pMsg));
	return true;
}

bool
MessageDecoder::createPicMetaDataObj(char *buffer,  ssize_t mlen, int fd)
{
	auto del = [] (PicMetaDataObj*){};
	PicMetaDataObj *pPicMeta = new PicMetaDataObj();
	std::unique_ptr<PicMetaDataObj> pMsg(pPicMeta, del);
	pMsg->setMsgTyp(PIC_METADATA_MSG);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setShrId(shareId);
	int nameLen;
	constexpr int namelenoffset = 2*sizeof(int) + sizeof(long);
	memcpy(&nameLen, buffer + namelenoffset, sizeof(int));
	int nameoffset = namelenoffset + sizeof(int);
	pMsg->setName(buffer + nameoffset, nameLen);	
	int piclenoffset = nameoffset + nameLen;
	int picLen;
	memcpy(&picLen, buffer + piclenoffset, sizeof(int));
	pMsg->setPicLen(picLen);
	int metastrlenoffset = piclenoffset + sizeof(int);
	int metaStrLen;
	memcpy(&metaStrLen, buffer + metastrlenoffset, sizeof(int));
	int metastroffset = metastrlenoffset + sizeof(int);
	pMsg->setFrndLstStr(buffer+metastroffset, metaStrLen);
	pMsgs.push_back(std::move(pMsg));
	return true;
}

bool
MessageDecoder::createLstObj(char *buffer,  ssize_t mlen, int fd)
{

	std::unique_ptr<LstObj> pMsg{new LstObj()};
	pMsg->setMsgTyp(SHARE_ITEM_MSG);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setShrId(shareId);
	int nameLen;
	constexpr int namelenoffset = 2*sizeof(int) + sizeof(long);
	memcpy(&nameLen, buffer + namelenoffset, sizeof(int));
	int lstLen;
	constexpr int lstlenoffset = 3*sizeof(int) + sizeof(long);
	memcpy(&lstLen, buffer + lstlenoffset, sizeof(int));
	constexpr int nameoffset = 4*sizeof(int) + sizeof(long);
	pMsg->setName(buffer + nameoffset, nameLen);	
	int lstoffset = 4*sizeof(int) + sizeof(long)+nameLen;
	if (lstLen)
		pMsg->setList(buffer+lstoffset, lstLen);	
	else
		return false;
	addMsgObj(std::move(pMsg));

	return true;
}

bool
MessageDecoder::createShareIdObj(char *buffer,  ssize_t mlen, int fd)
{
      std::unique_ptr<ShareIdObj> pMsg{new ShareIdObj()};	
      pMsg->setMsgTyp(GET_SHARE_ID_MSG);
	constexpr int offset = 2*sizeof(int);
	long tid;
	memcpy(&tid, buffer+offset, sizeof(long));
	pMsg->setTrnId(tid);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
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
	pMsg->setAppId(getAppId());
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
	std::unique_ptr<FrndLstObj> pMsg{new FrndLstObj()};
	pMsg->setMsgTyp(STORE_FRIEND_LIST_MSG);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
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

bool
MessageDecoder::createTemplLstObj(char *buffer,  ssize_t mlen, int fd)
{
	std::unique_ptr<TemplLstObj> pMsg{new TemplLstObj()};

	pMsg->setMsgTyp(ARCHIVE_ITEM_MSG);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setShrId(shareId);
	int nameLen;
	constexpr int namelenoffset = 2*sizeof(int) + sizeof(long);
	memcpy(&nameLen, buffer + namelenoffset, sizeof(int));
	int templLen;
	constexpr int templlenoffset = 3*sizeof(int) + sizeof(long);
	memcpy(&templLen, buffer + templlenoffset, sizeof(int));
	constexpr int nameoffset = 4*sizeof(int) + sizeof(long);
	pMsg->setName(buffer + nameoffset, nameLen);	
	int templlstoffset = 4*sizeof(int) + sizeof(long)+nameLen;
	if (templLen)
		pMsg->setTemplList(buffer+templlstoffset, templLen);	
	else
		return false;
	pMsgs.push_back(std::move(pMsg));
	return true;
}

bool
MessageDecoder::createDeviceTknObj(char *buffer, ssize_t mlen, int fd)
{
	std::unique_ptr<DeviceTknObj> pMsg = std::unique_ptr<DeviceTknObj>{new DeviceTknObj()};
	pMsg->setMsgTyp(STORE_DEVICE_TKN_MSG);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int devTknOffset = offset + sizeof(long);
	pMsg->setDeviceTkn(buffer + devTknOffset);
	int devIdOffset = devTknOffset + pMsg->getDeviceTkn().size() + 1;
	pMsg->setDeviceId(buffer + devIdOffset);
	pMsgs.push_back(std::move(pMsg));
	return true;
}


bool
MessageDecoder::createGetItemObj(char *buffer, ssize_t mlen, int fd)
{
	std::unique_ptr<GetItemObj> pMsg = std::unique_ptr<GetItemObj>{new GetItemObj()};
	pMsg->setMsgTyp(GET_ITEMS);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int devIdOffset = offset + sizeof(long);
	pMsg->setDeviceId(buffer + devIdOffset);
	pMsgs.push_back(std::move(pMsg));
	return true;
}


