#include <EasyGrocDecoder.h>
#include <functional>
#include <iostream>
#include <string.h>

using namespace std::placeholders;

EasyGrocDecoder::EasyGrocDecoder()
{
	for (int i=0; i < NO_EASYGROC_MSGS; ++i)
		msgTypPrcsrs[i] = -1;
	msgTypPrcsrs[STORE_TEMPL_LIST_MSG%NO_EASYGROC_MSGS] = 0;
	msgTypPrcsrs[SHARE_LIST_MSG%NO_EASYGROC_MSGS] = 1;
	msgTypPrcsrs[STORE_EASYGROC_DEVICE_TKN_MSG%NO_EASYGROC_MSGS] = 2;
	msgTypPrcsrs[GET_EASYGROC_ITEMS%NO_EASYGROC_MSGS] = 3;
}

EasyGrocDecoder::~EasyGrocDecoder()
{

}

bool
EasyGrocDecoder::decodeMsg(char *buffer, ssize_t mlen, int fd)
{
	static auto processors = {std::bind(std::mem_fn(&EasyGrocDecoder::createTemplLstObj), this, _1, _2, _3), std::bind(std::mem_fn(&EasyGrocDecoder::createLstObj), this, _1, _2, _3),
std::bind(std::mem_fn(&EasyGrocDecoder::createDeviceTknObj), this, _1, _2, _3),
std::bind(std::mem_fn(&EasyGrocDecoder::createGetItemObj), this, _1, _2, _3)};
	int msgTyp;
	memcpy(&msgTyp, buffer+sizeof(int), sizeof(int));
	if (msgTyp < EASY_GROC_MSG_START || msgTyp > EASY_GROC_MSG_END)
	{
		std::cout << "Invalid message type " << msgTyp << " received in EasyGrocDecoder " << std::endl;
		return false;		
	}
	int pindx = msgTypPrcsrs[msgTyp%NO_EASYGROC_MSGS];
       if (pindx == -1) 
                return false;
        auto itr = processors.begin();
        return itr[pindx](buffer, mlen, fd);	
	
}

bool
EasyGrocDecoder::createLstObj(char *buffer,  ssize_t mlen, int fd)
{

	std::unique_ptr<LstObj> pMsg{new LstObj()};
	pMsg->setMsgTyp(SHARE_LIST_MSG);
	pMsg->setFd(fd);
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
EasyGrocDecoder::createTemplLstObj(char *buffer,  ssize_t mlen, int fd)
{
	std::unique_ptr<TemplLstObj> pMsg{new TemplLstObj()};

	pMsg->setMsgTyp(STORE_TEMPL_LIST_MSG);
	pMsg->setFd(fd);
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
	addMsgObj(std::move(pMsg));
	return true;
}

bool
EasyGrocDecoder::createDeviceTknObj(char *buffer, ssize_t mlen, int fd)
{
	std::unique_ptr<DeviceTknObj> pMsg = std::unique_ptr<DeviceTknObj>{new DeviceTknObj()};
	pMsg->setMsgTyp(STORE_EASYGROC_DEVICE_TKN_MSG);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setFd(fd);
	constexpr int devTknOffset = offset + sizeof(long);
	pMsg->setDeviceTkn(buffer + devTknOffset);
	int devIdOffset = devTknOffset + pMsg->getDeviceTkn().size() + 1;
	pMsg->setDeviceId(buffer + devIdOffset);
	addMsgObj(std::move(pMsg));
	return true;
}


bool
EasyGrocDecoder::createGetItemObj(char *buffer, ssize_t mlen, int fd)
{
	std::unique_ptr<GetItemObj> pMsg = std::unique_ptr<GetItemObj>{new GetItemObj()};
	pMsg->setMsgTyp(GET_EASYGROC_ITEMS);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setFd(fd);
	constexpr int devIdOffset = offset + sizeof(long);
	pMsg->setDeviceId(buffer + devIdOffset);
	addMsgObj(std::move(pMsg));
	return true;
}


