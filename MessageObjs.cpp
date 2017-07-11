#include <MessageObjs.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/time.h>

void
MsgObjDeltr::operator () (MsgObj *pMsg)
{
	PicObj *pPicObj = dynamic_cast<PicObj*>(pMsg);	
	if (pPicObj)
		return;
	PicMetaDataObj *pPicMetaObj = dynamic_cast<PicMetaDataObj*>(pMsg);
	if (pPicMetaObj)
		return;
	delete pMsg;
	return;
}


void
PicMetaDataObj::setName(const char *pName, int len)
{
	name.resize(len);
	memcpy(&name[0], pName, len);
	return;
}

std::ostream&
operator << (std::ostream& os, const devTknArchv& devTkInfo)
{
	os << " appId=" << devTkInfo.appId << " shareId=" << devTkInfo.shareId << " tkn_len=" << devTkInfo.tkn_len << " platform=" << devTkInfo.platform ;
	return os;
}


std::ostream&
operator << (std::ostream& os, const shareInfo& shInfo)
{
    os << " appId=" << shInfo.appId << " shrIdLst=" << shInfo.shrIdLst << " shrId=" << shInfo.shrId << " del=" << shInfo.del << " name_len=" << shInfo.name_len << " list_len=" << shInfo.list_len;
    return os;
}

std::ostream&
operator << (std::ostream& os, const GetItemObj& getItem)
{
	os << " appId=" << getItem.getAppId() << " shrId=" << getItem.getShrId() << " devId=" << getItem.getDeviceId() << " fd=" << getItem.getFd();
	return os;
}


void
PicMetaDataObj::setFrndLstStr(const char *pFlist, int len)
{
	frndLstStr.resize(len);
	memcpy(&frndLstStr[0], pFlist, len);
	std::istringstream ifs(frndLstStr);		
	std::string s;
	while(std::getline(ifs, s, ';'))
	{
		std::cout << "friend shareId=" << s << " size=" << s.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		if (s.size() > 3)
			frndLst.push_back(s);
	}
	std::cout << "frndLst size=" << frndLst.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return;
}

PicObj::PicObj()
{
	len = 0;
}

void
PicObj::setBuf(char *pb, int pblen)
{
	memcpy(buf, pb, pblen);
	buflen = pblen;
}

void
TemplLstObj::setName(const char *pName, int len)
{
	name.resize(len);
	memcpy(&name[0], pName, len);
	return;
}

void
TemplLstObj::setTemplList(const char *pList, int len)
{
	templList.resize(len);
	memcpy(&templList[0], pList, len);
	return;
}

void
LstObj::setName(const char *pName, int len)
{
	name.resize(len);
	memcpy(&name[0], pName, len);
	return;
}

void
LstObj::setList(const char *pList, int len)
{
	list.resize(len);
	memcpy(&list[0], pList, len);
	return;
}
