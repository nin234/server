#include <MessageObjs.h>
#include <string.h>

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
