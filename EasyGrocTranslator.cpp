#include <EasyGrocTranslator.h>
#include <iostream>
#include <sstream>
#include <Constants.h>
#include <string.h>

EasyGrocTranslator::EasyGrocTranslator()
{

}

EasyGrocTranslator::~EasyGrocTranslator()
{

}

bool
EasyGrocTranslator::translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj)
{

	return true;
}

bool 
EasyGrocTranslator::getShareIds(const std::string& lst, std::vector<std::string>& shareIds)
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
EasyGrocTranslator::getListMsg(char *buf, int *mlen, int buflen, const std::string& name, const std::string& lst1)
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
	

	constexpr int msgId = GET_EASYGROC_LIST_MSG;
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

