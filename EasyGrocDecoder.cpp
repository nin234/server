#include <EasyGrocDecoder.h>
#include <functional>
#include <iostream>
#include <string.h>

using namespace std::placeholders;

EasyGrocDecoder::EasyGrocDecoder()
{
    for (auto &msgTypPrc : msgTypPrcsrs)
        msgTypPrc = -1;
    
    msgTypPrcsrs[SHARE_TEMPL_ITEM_MSG%NO_EASYGROC_MSGS] = 0;
}

EasyGrocDecoder::~EasyGrocDecoder()
{

}

bool
EasyGrocDecoder::decodeMsg(char *buffer, ssize_t mlen, int fd)
{
    int msgTyp;
    memcpy(&msgTyp, buffer+sizeof(int), sizeof(int));
    static auto processors = {
        std::bind(std::mem_fn(&EasyGrocDecoder::createShareTemplLstObj), this, _1, _2, _3)
    };
    
    if (msgTyp > EASY_GROC_MSG_END || msgTyp < EASY_GROC_MSG_START)
    {
        std::cout << "Invalid message received msgTyp=" << msgTyp << std::endl;
        return false;
    }
        
    int pindx = msgTypPrcsrs[msgTyp];
    if (pindx == -1)
    {
        std::cout << "No handler found for msgTyp=" << msgTyp << std::endl;
        return false;
    }
    auto itr = processors.begin();
    return itr[pindx](buffer, mlen, fd);

}

bool
EasyGrocDecoder::createShareTemplLstObj(char *buffer,  ssize_t mlen, int fd)
{
    std::unique_ptr<TemplLstObj, MsgObjDeltr> pMsg{new TemplLstObj(), MsgObjDeltr()};
    
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
        pMsg->setTemplList(buffer+templlstoffset, templLen);
    else
        return false;
    addMsgObj(std::move(pMsg));
    return true;
}



