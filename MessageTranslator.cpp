#include <MessageTranslator.h>
#include <string.h>
#include <Constants.h>

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

std::unique_ptr<char>
MessageTranslator::getListMsg(char *buf, int *mlen, int buflen, const std::string& name, const std::string& lst1)
{
	std::string lst;
	std::string::size_type xpos = lst.find(":::");
	if (xpos == std::string::npos)
	{
		lst = lst1;
	}
	else
	{
		lst = lst1.substr(xpos+3);
	}
	int msglen = 4*sizeof(int) + name.size() + lst.size() + 2;
	std::unique_ptr<char> pMsgDynamic;
	char *pMsg;
	if (buflen >= msglen)
		pMsg = buf;
	else
	{
		std::unique_ptr<char> tmp{new char[msglen]};
		pMsgDynamic = std::move(tmp);
		pMsg = pMsgDynamic.get();
		std::cout << "Dynamic allocation in getListMsg " << name << std::endl;
	}
	

	constexpr int msgId = GET_ITEMS_RPLY_MSG;
	memcpy(pMsg, &msglen, sizeof(int));
	memcpy(pMsg+sizeof(int), &msgId, sizeof(int));
	int namelen = name.size() + 1;
	int lstlen = lst.size() + 1;
	memcpy(pMsg+2*sizeof(int), &namelen, sizeof(int));
	memcpy(pMsg+3*sizeof(int), &lstlen, sizeof(int));
	memcpy(pMsg + 4*sizeof(int), name.c_str(), namelen);
	int lstoffset = 4*sizeof(int) + namelen;
	memcpy(pMsg+lstoffset, lst.c_str(), lstlen);
	*mlen = msglen;
	return pMsgDynamic;
}

