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
MessageTranslator::getShareIdReply(char *buf, int* mlen, long shareId)
{
	constexpr int len = 2*sizeof(int) + sizeof(long);
	constexpr int shoffset = 2*sizeof(int);
	*mlen = len;
	memcpy(buf, &len, sizeof(int));
	constexpr int msgId = GET_SHARE_ID_RPLY_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	memcpy(buf+shoffset, &shareId, sizeof(long));
	return true;
}

bool
MessageTranslator::getStoreIdReply(char *buf, int* mlen)
{
	constexpr int len = 2*sizeof(int);
	memcpy(buf, &len, sizeof(int));
	constexpr int msgId = STORE_TRNSCTN_ID_RPLY_MSG;
	memcpy(buf+sizeof(int), &msgId, sizeof(int));
	return true;
}



bool
MessageTranslator::getReply(char *buf, int* mlen, int msgTyp)
{
	constexpr int len = 2*sizeof(int);
	memcpy(buf, &len, sizeof(int));
	memcpy(buf+sizeof(int), &msgTyp, sizeof(int));
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
MessageTranslator::getPicMetaMsg(char *pMsg, int *mlen, int buflen, const shrIdLstName& shidlst)
{
	int msglen = 2*sizeof(long) + 3*sizeof(int) + shidlst.lstName.size() +1;
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
	*mlen = msglen;
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

