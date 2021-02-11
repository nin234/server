#include <MessageTranslator.h>
#include <string.h>
#include <Constants.h>
#include <iostream>
#include <sstream>

MessageTranslator::MessageTranslator()
{

}

MessageTranslator::~MessageTranslator()
{

}

bool
MessageTranslator::getUpdateShareIdMsg(char *buf, int *mlen, long shareId)
{
    constexpr int len = 2*sizeof(int) + sizeof(long);
	constexpr int shoffset = 2*sizeof(int);
	*mlen = len;
	memcpy(buf, &len, sizeof(int));
	constexpr int msgId = UPDATE_MAX_SHARE_ID_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	memcpy(buf+shoffset, &shareId, sizeof(long));
    return true;
}

bool
MessageTranslator::getShareIdReply(char *buf, int* mlen, long shareId)
{
	constexpr int len = 2*sizeof(int) + sizeof(long);
	constexpr int shoffset = 2*sizeof(int);
	constexpr int msgId = GET_SHARE_ID_RPLY_MSG;
	*mlen = len;
	memcpy(buf, &len, sizeof(int));
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	memcpy(buf+shoffset, &shareId, sizeof(long));
	return true;
}

bool
MessageTranslator::getTotalPicLenMsg(char *buf, int* mlen, long picLen)
{
	constexpr int len = 2*sizeof(int) + sizeof(long);
	constexpr int shoffset = 2*sizeof(int);
	*mlen = len;
	memcpy(buf, &len, sizeof(int));
	constexpr int msgId = TOTAL_PIC_LEN_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	memcpy(buf+shoffset, &picLen, sizeof(long));
	return true;
}


bool
MessageTranslator::getStoreIdReply(char *buf, int* mlen)
{
	constexpr int len = 2*sizeof(int);
	memcpy(buf, &len, sizeof(int));
	constexpr int msgId = STORE_TRNSCTN_ID_RPLY_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	*mlen = len;
	return true;
}



bool
MessageTranslator::getReply(char *buf, int* mlen, int msgTyp)
{
	constexpr int len = 2*sizeof(int);
	memcpy(buf, &len, sizeof(int));
	memcpy(buf+sizeof(int), &msgTyp, sizeof(int));
	*mlen = len;
	return true;
}



bool 
MessageTranslator::getShareIds(const std::string& lst, std::vector<std::string>& shareIds)
{
	std::string::size_type xpos = lst.find(":::");
	if (xpos == std::string::npos)
		return false;
	std::string shareIdStr = lst.substr(0, xpos);
	std::cout << " List of share Ids " << shareIdStr << std::endl;
	std::istringstream ifs(shareIdStr);		
	std::string s;
	while(std::getline(ifs, s, ';'))
	{
		std::cout << s << std::endl;
		shareIds.push_back(s);
	}
	return true;
}



bool
MessageTranslator::getFrndLstMsg(char *pMsg, int *mlen, int buflen, const std::string& frndLst)
{
	int msglen = 3*sizeof(int) + frndLst.size() +1;
	if (buflen < msglen)
	{
		std::cout << "buflen=" << buflen << " less than msglen=" << msglen << " in MessageTranslator:getFrndLstMsg " << std::endl;
		return false;
	}
	constexpr int msgId = FRIEND_LIST_MSG;
	memcpy(pMsg, &msglen, sizeof(int));
	memcpy(pMsg+sizeof(int), &msgId, sizeof(int));
    int frndLstLen = frndLst.size() + 1;
    memcpy(pMsg+2*sizeof(int), &frndLstLen, sizeof(int));
    memcpy(pMsg + 3*sizeof(int), frndLst.c_str(), frndLstLen); 
    *mlen = msglen;
    return true;
}

bool
MessageTranslator::getShareIdRemoteHostMsg(char *buf, int *mlen, std::string host , int port)
{

	int msglen = 4*sizeof(int) + host.size() +1;
	memcpy(buf, &msglen, sizeof(int));
	constexpr int msgId = SHARE_ID_REMOTE_HOST_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
    int hostlen = host.size() + 1;
	memcpy(buf+2*sizeof(int), &hostlen, sizeof(int));
	memcpy(buf + 3*sizeof(int), host.c_str(), hostlen);
    int portOffset = 3*sizeof(int) + hostlen;
    memcpy(buf + portOffset, &port, sizeof(int));
    *mlen = msglen;
    return true;
}

bool
MessageTranslator::getPicMetaMsg(char *pMsg, int *mlen, int buflen, const shrIdLstName& shidlst)
{
	int msglen = 2*sizeof(long) + 4*sizeof(int) + shidlst.lstName.size() +1;
	if (buflen < msglen)
	{
		std::cout << "buflen=" << buflen << " less than msglen=" << msglen << " in MessageTranslator:getPicMetaMsg " << std::endl;
		return false;
	}
	constexpr int msgId = PIC_METADATA_MSG;
	memcpy(pMsg, &msglen, sizeof(int));
	memcpy(pMsg+sizeof(int), &msgId, sizeof(int));
	memcpy(pMsg+2*sizeof(int), &shidlst.shareId, sizeof(long));
	int picNameLen = shidlst.lstName.size() + 1;
	int picNameLenOffset = 2*sizeof(int) + sizeof(long);
	memcpy(pMsg+picNameLenOffset, &picNameLen, sizeof(int));
	int picNameOffset = picNameLenOffset+sizeof(int);	
	memcpy(pMsg+picNameOffset, shidlst.lstName.c_str(), picNameLen);
	int picLenOffset = picNameOffset+picNameLen;
	memcpy(pMsg+picLenOffset, &shidlst.picLen, sizeof(long));
	int picOffset = picLenOffset + sizeof(long);
	memcpy(pMsg + picOffset, &shidlst.picSoFar, sizeof(int));
	*mlen = msglen;
	return true;
}
bool 
MessageTranslator::createServerPicMetaMsg(std::vector<char>& msg, std::shared_ptr<PicMetaDataObj> pPicMetaObj, const std::vector<std::string>& shareIds)
{

	constexpr int msgId = PIC_METADATA_MSG;
    std::string name = pPicMetaObj->getName();
    int nameLen = name.size() + 1;
    std::stringstream ss;
    for (auto& shareId : shareIds)
    {
        ss << shareId << ";";
    }
    std::string metaStr = ss.str();
    int metaStrLen = metaStr.size() + 1;
    int msglen = 5*sizeof(int) + nameLen  + sizeof(long) + metaStrLen;
    msg.reserve(msglen);    
    char* pMsg = msg.data();
	memcpy(pMsg, &msglen, sizeof(int));
	memcpy(pMsg+sizeof(int), &msgId, sizeof(int));
    long shareId = pPicMetaObj->getShrId();
    memcpy(pMsg + 2*sizeof(int), &shareId, sizeof(long));
    constexpr int namelenoffset = 2*sizeof(int) + sizeof(long);
    memcpy(pMsg + namelenoffset, &nameLen, sizeof(int));
    int nameoffset = namelenoffset + sizeof(int);
    memcpy(pMsg + nameoffset, name.c_str(), nameLen);
    int piclenoffset = nameoffset + nameLen;
    int picLen = pPicMetaObj->getPicLen();
    memcpy(pMsg + piclenoffset, &picLen, sizeof(int)); 
    int metastrlenoffset = piclenoffset + sizeof(int);
    memcpy(pMsg + metastrlenoffset, &metaStrLen, sizeof(int));
    int metastroffset = metastrlenoffset + sizeof(int);
    memcpy(pMsg + metastroffset, metaStr.c_str(), metaStrLen);
    return true;
}


bool 
MessageTranslator::createShareItemMsg(std::vector<char>& msg, std::shared_ptr<LstObj> pLstObj, const std::vector<std::string>& shareIds)
{

	std::string::size_type xpos = pLstObj->getList().find(":::");
	if (xpos == std::string::npos)
    {
        std::cout << "Invalid list format for list=" << pLstObj->getList() << " " << __FILE__ << ":" << __LINE__ << std::endl;  
		return false;
    }
    std::string lst = pLstObj->getList();
    lst.erase(0, xpos);
    std::stringstream ss;
    for (const auto& shareId : shareIds)
    {
        ss << shareId << ";"; 
    }
    std::string pre = ss.str();
    std::string prefix = pre.substr(0, pre.size()-1);
    std::string lstShare = prefix + lst;
    int listLen = lstShare.size()+1;
     int nameLen =  pLstObj->getName().size() + 1;
    int msglen = 4*sizeof(int) + nameLen + listLen + sizeof(long);
    
    msg.reserve(msglen);    
    auto pMsg = std::make_unique<char[]>(msglen);

	memcpy(pMsg.get(), &msglen, sizeof(int));
    int msgId = SHARE_ITEM_MSG;
	memcpy(pMsg.get()+sizeof(int), &msgId, sizeof(int));
    long shareId = pLstObj->getShrId();
    memcpy(pMsg.get() + 2*sizeof(int), &shareId, sizeof(long)); 
    constexpr int namelenoffset = 2*sizeof(int) + sizeof(long);
    memcpy(pMsg.get() + namelenoffset, &nameLen, sizeof(int));
     constexpr int lstlenoffset = 3*sizeof(int) + sizeof(long);
    memcpy(pMsg.get() + lstlenoffset, &listLen, sizeof(int));
    constexpr int nameoffset = 4*sizeof(int) + sizeof(long);
    memcpy(pMsg.get() + nameoffset, pLstObj->getName().c_str(), nameLen);    
    int lstoffset = 4*sizeof(int) + sizeof(long)+nameLen;
	memcpy(pMsg.get()+lstoffset, lstShare.c_str(), listLen);
    msg.assign(pMsg.get(), pMsg.get()+msglen); 
    return true;
}

bool
MessageTranslator::getListMsg(char *pMsg, int *mlen, int buflen, const std::string& name, const std::string& lst, int msgId, long shareId)
{
	int msglen = 4*sizeof(int) + name.size() + lst.size() + 2 + sizeof(long);
	if (buflen < msglen)
	{
		std::cout << "buflen=" << buflen << " less than msglen=" << msglen << " in MessageTranslator:getListMsg " << std::endl;
		return false;
	}

	
	memcpy(pMsg, &msglen, sizeof(int));
	memcpy(pMsg+sizeof(int), &msgId, sizeof(int));
	memcpy(pMsg +2*sizeof(int), &shareId, sizeof(long));
	int namelen = name.size() + 1;
	int lstlen = lst.size() + 1;
	memcpy(pMsg+2*sizeof(int) + sizeof(long), &namelen, sizeof(int));
	memcpy(pMsg+3*sizeof(int) + sizeof(long), &lstlen, sizeof(int));
	memcpy(pMsg + 4*sizeof(int) + sizeof(long), name.c_str(), namelen);
	int lstoffset = 4*sizeof(int) + sizeof(long) + namelen;
	memcpy(pMsg+lstoffset, lst.c_str(), lstlen);
	*mlen = msglen;
	return true;
}

bool
MessageTranslator::getShouldUploadMsg(char *pMsg, int *mlen, std::shared_ptr<PicMetaDataObj> pPicMetaObj, bool shouldUpload, int picOffset)
{
	int msglen = 5*sizeof(int) + sizeof(long) + pPicMetaObj->getName().size() + 1;
	int msgId = SHOULD_UPLOAD_MSG;
	memcpy(pMsg, &msglen, sizeof(int));
	memcpy(pMsg+sizeof(int), &msgId, sizeof(int));
	long shareId = pPicMetaObj->getShrId();
	memcpy(pMsg +2*sizeof(int), &shareId, sizeof(long));
	int namelen = pPicMetaObj->getName().size() + 1;
	int upload = shouldUpload ? 1: 0; 
	memcpy(pMsg+2*sizeof(int) + sizeof(long), &namelen, sizeof(int));
	memcpy(pMsg+3*sizeof(int) + sizeof(long), &upload, sizeof(int));
	memcpy(pMsg+4*sizeof(int) + sizeof(long), &picOffset, sizeof(int));
	memcpy(pMsg + 5*sizeof(int) + sizeof(long), pPicMetaObj->getName().c_str(), namelen);
	*mlen = msglen;
	return true;
}
