#include <MessageDecoder.h> 
#include <string.h>
#include <iostream>
#include <functional>
#include <Util.h>

using namespace std::placeholders;

MessageDecoder::MessageDecoder()
{

}

MessageDecoder::~MessageDecoder()
{

}



bool 
MessageDecoder::operator()(char* buffer, ssize_t mlen, int fd)
{
    	int msgTyp;
    	memcpy(&msgTyp, buffer+sizeof(int), sizeof(int));
		decodeMsg(buffer, mlen, fd);
	
	switch(msgTyp)
	{
		case GET_SHARE_ID_MSG:
		 	return createShareIdObj(buffer, mlen, fd);

		case STORE_TRNSCTN_ID_MSG:
			return createStoreIdObj(buffer, mlen, fd);

		case STORE_FRIEND_LIST_MSG:
			return createFrndLstObj(buffer, mlen, fd);

		case ARCHIVE_ITEM_MSG:
			return createTemplLstObj(buffer, mlen, fd);

		case SHARE_ITEM_MSG:
			return createLstObj(buffer, mlen, fd);

		case STORE_DEVICE_TKN_MSG:
			return createDeviceTknObj(buffer, mlen, fd);

		case GET_ITEMS:
			return createGetItemObj(buffer, mlen, fd);

		case PIC_METADATA_MSG:
			return createPicMetaDataObj(buffer, mlen, fd);

		case PIC_MSG:
			return createPicObj(buffer, mlen, fd);

		case PIC_DONE_MSG:
			return createPicDoneObj(buffer, mlen, fd);

		case SHOULD_DOWNLOAD_MSG:
			return createShouldDownLoadObj(buffer, mlen, fd);

        case GET_REMOTE_HOST_MSG:
			return createGetRemoteHost(buffer, mlen, fd);
 
		default:
//			std::cout << "Unhandled message msgTyp=" << msgTyp << " " << __FILE__ << ":" << __LINE__ << std::endl;		
		break;
	}	
   	return false; 
}

bool
MessageDecoder::createGetRemoteHost(char *buffer,  ssize_t mlen, int fd)
{
    std::shared_ptr<GetRemoteHostObj> pMsg = std::make_shared<GetRemoteHostObj>();
	pMsg->setMsgTyp(GET_REMOTE_HOST_MSG);	
	pMsg->setFd(fd);
    int appId;
    memcpy(&appId, buffer + 2*sizeof(int), sizeof(int)); 
	pMsg->setAppId(appId);
    long shareId;
    memcpy(&shareId, buffer + 3*sizeof(int), sizeof(long));
    pMsg->setShareId(shareId);
	pMsgs.push_back(pMsg);
    return true;
}

bool
MessageDecoder::createPicObj(char *buffer,  ssize_t mlen, int fd)
{
	std::shared_ptr<PicObj> pMsg = std::make_shared<PicObj>();
	pMsg->setMsgTyp(PIC_MSG);	
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	pMsg->setBuf(buffer+2*sizeof(int), mlen-2*sizeof(int));
	int len;
	memcpy(&len, buffer, sizeof(int));
	pMsg->setLen(len);
	pMsgs.push_back(pMsg);
	return true;
}

bool
MessageDecoder::createPicMetaDataObj(char *buffer,  ssize_t mlen, int fd)
{
	std::shared_ptr<PicMetaDataObj> pMsg = std::make_shared<PicMetaDataObj>();
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
	pMsgs.push_back(pMsg);
	return true;
}

bool
MessageDecoder::createLstObj(char *buffer,  ssize_t mlen, int fd)
{

	std::shared_ptr<LstObj> pMsg = std::make_shared<LstObj>();
	pMsg->setMsgTyp(SHARE_ITEM_MSG);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	int msgLen;
	memcpy(&msgLen, buffer, sizeof(int));
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
	std::cout << "Creating list item lstoffset=" << lstoffset << " nameoffset=" << nameoffset << " nameLen=" << nameLen << " lstLen=" << lstLen << " msgLen=" << msgLen  << " " << __FILE__ << ":" << __LINE__ << std::endl;
	if (lstLen)
		pMsg->setList(buffer+lstoffset, lstLen);	
	else
		return false;
	addMsgObj(pMsg);

	return true;
}

bool
MessageDecoder::createShareIdObj(char *buffer,  ssize_t mlen, int fd)
{
	std::shared_ptr<ShareIdObj> pMsg = std::make_shared<ShareIdObj>();
      pMsg->setMsgTyp(GET_SHARE_ID_MSG);
    int msgLen;
    memcpy(&msgLen, buffer, sizeof(int));
	constexpr int offset = 2*sizeof(int);

    constexpr int lenWithOutId = 2*sizeof(int) + sizeof(long);
	long tid;
	memcpy(&tid, buffer+offset, sizeof(long));
	pMsg->setTrnId(tid);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
    if (msgLen > lenWithOutId)
    {
        pMsg->setDeviceId(buffer+lenWithOutId);
    }
	std::cout << "Creating shareId Obj fd=" << fd << " tid=" << tid << " appId=" << getAppId() << " deviceId=" << pMsg->getDeviceId()
     << " " << __FILE__ << " " << __LINE__ << std::endl;
	pMsgs.push_back(pMsg);
    return true;
}

bool
MessageDecoder::createStoreIdObj(char *buffer,  ssize_t mlen, int fd)
{
	std::shared_ptr<ShareIdObj> pMsg = std::make_shared<ShareIdObj>();
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
	pMsgs.push_back(pMsg);
    return true;
}

std::shared_ptr<MsgObj>
MessageDecoder::getNextMsg()
{
	if (!pMsgs.empty())
	{
		std::shared_ptr<MsgObj> pMsg = pMsgs.front();
		pMsgs.pop_front();
		return pMsg;
	}
	return std::shared_ptr<MsgObj>();
}

bool
MessageDecoder::createFrndLstObj(char *buffer, ssize_t mlen, int fd)
{
	std::shared_ptr<FrndLstObj> pMsg = std::make_shared<FrndLstObj>();
	pMsg->setMsgTyp(STORE_FRIEND_LIST_MSG);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setShrId(shareId);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int frndLstOffset = offset + sizeof(long);
	pMsg->setFrndLst(buffer + frndLstOffset);
	pMsgs.push_back(pMsg);
	return true;
}

void
MessageDecoder::addMsgObj(std::shared_ptr<MsgObj> pMsg)
{
	pMsgs.push_back(pMsg);
}

bool
MessageDecoder::createTemplLstObj(char *buffer,  ssize_t mlen, int fd)
{
	std::shared_ptr<TemplLstObj> pMsg = std::make_shared<TemplLstObj>();
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
	pMsgs.push_back(pMsg);
	return true;
}

bool
MessageDecoder::createDeviceTknObj(char *buffer, ssize_t mlen, int fd)
{
	std::shared_ptr<DeviceTknObj> pMsg = std::make_shared<DeviceTknObj>();
	pMsg->setMsgTyp(STORE_DEVICE_TKN_MSG);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setShrId(shareId);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int devTknOffset = offset + sizeof(long);
	pMsg->setDeviceTkn(buffer + devTknOffset);
	int devIdOffset = devTknOffset + pMsg->getDeviceTkn().size() + 1;
	pMsg->setPlatform(buffer + devIdOffset);
	pMsgs.push_back(pMsg);
	return true;
}

bool
MessageDecoder::createPicDoneObj(char *buffer, ssize_t mlen, int fd)
{
	std::shared_ptr<PicDoneObj> pMsg = std::make_shared<PicDoneObj>();
	pMsg->setMsgTyp(PIC_DONE_MSG);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setShrId(shareId);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	long picShareId =0;
	int picshidoffset = offset + sizeof(long);
	memcpy(&picShareId, buffer+picshidoffset, sizeof(long));
	int picnameoffset = picshidoffset + sizeof(long);
	pMsg->setPicShareId(picShareId);
	pMsg->setPicName(buffer + picnameoffset);	
	pMsgs.push_back(pMsg);
	return true;
}

bool
MessageDecoder::createShouldDownLoadObj(char *buffer, ssize_t mlen, int fd)
{
	std::cout << "Decoding createShouldDownLoadObj " << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	std::shared_ptr<ShouldDownLoad> pMsg = std::make_shared<ShouldDownLoad>();
    constexpr int offset = 2*sizeof(int);
    long shareId;
    memcpy(&shareId, buffer+offset, sizeof(long));
    pMsg->setShrId(shareId);
    pMsg->setMsgTyp(SHOULD_DOWNLOAD_MSG);
    pMsg->setFd(fd);
    pMsg->setAppId(getAppId());
    int download;
    memcpy(&download, buffer+offset+sizeof(long), sizeof(int));
    if (download)
    {
        pMsg->setDownLoad(true);
    }
    else
    {
        pMsg->setDownLoad(false);
    }
    int nameoffset = offset + sizeof(long) + sizeof(int);
    pMsg->setName(buffer + nameoffset);
    pMsgs.push_back(pMsg);
    return true;
}

bool
MessageDecoder::createGetItemObj(char *buffer, ssize_t mlen, int fd)
{
	std::shared_ptr<GetItemObj> pMsg = std::make_shared<GetItemObj>();
	pMsg->setMsgTyp(GET_ITEMS);
	constexpr int offset = 2*sizeof(int);
	long shareId;
	memcpy(&shareId, buffer+offset, sizeof(long));
	pMsg->setShrId(shareId);
	pMsg->setFd(fd);
	pMsg->setAppId(getAppId());
	constexpr int devIdOffset = offset + sizeof(long);
	pMsg->setDeviceId(buffer + devIdOffset);
	int picrmngoffset = devIdOffset + pMsg->getDeviceId().size() +1;
	int picRemaining=0;
	memcpy(&picRemaining, buffer+picrmngoffset, sizeof(int));
	pMsg->setPicRemaining(picRemaining);
	pMsg->setPicName(buffer+picrmngoffset+sizeof(int));
	int picshidoffset = picrmngoffset + sizeof(int) + pMsg->getPicName().size() + 1;
	long picShareId = 0;
	memcpy(&picShareId, buffer+picshidoffset, sizeof(long));
	pMsg->setPicShareId(picShareId);
    long maxShareId = 0;
    int maxShIdOffset = picshidoffset + sizeof(long);
    ssize_t len = maxShIdOffset + sizeof(long);
    if (mlen >= len)
    {
        memcpy(&maxShareId, buffer+maxShIdOffset, sizeof(long));
        pMsg->setMaxShareId(maxShareId);
    }
	std::cout << Util::now() << "Create getItemObj " << *pMsg << " appId=" << getAppId() << __FILE__ << ":" << __LINE__ << std::endl;	
	
	pMsgs.push_back(pMsg);
	return true;
}


