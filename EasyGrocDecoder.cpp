#include <EasyGrocDecoder.h>
#include <functional>
#include <iostream>
#include <string.h>

using namespace std::placeholders;

EasyGrocDecoder::EasyGrocDecoder()
{
}

EasyGrocDecoder::~EasyGrocDecoder()
{

}

bool
EasyGrocDecoder::decodeMsg(char *buffer, ssize_t mlen, int fd)
{
    int msgTyp;
    memcpy(&msgTyp, buffer+sizeof(int), sizeof(int));
    switch(msgTyp)
    {
	case SHARE_TEMPL_ITEM_MSG:
		return createShareTemplLstObj(buffer, mlen, fd);
	default:
		std::cout << "Unhandled MsgTyp=" << msgTyp << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	break;
    }
    return false;

}

bool
EasyGrocDecoder::createShareTemplLstObj(char *buffer,  ssize_t mlen, int fd)
{
	std::shared_ptr<TemplLstObj> pMsg = std::make_shared<TemplLstObj>();
    
    pMsg->setMsgTyp(SHARE_TEMPL_ITEM_MSG);
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
    {
        pMsg->setTemplList(buffer+templlstoffset, templLen);
    }
    else
    {
        return false;
    }
	addMsgObj(pMsg);
    return true;
}



