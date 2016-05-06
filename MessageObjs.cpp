#include <MessageObjs.h>
#include <string.h>
#include <iostream>
#include <sstream>

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

void
PicMetaDataObj::setFrndLstStr(const char *pFlist, int len)
{
	frndLstStr.resize(len);
	memcpy(&frndLstStr[0], pFlist, len);
	std::istringstream ifs(frndLstStr);		
	std::string s;
	while(std::getline(ifs, s, ';'))
	{
		std::cout << s << std::endl;
		frndLst.push_back(s);
	}
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
