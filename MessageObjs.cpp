#include <MessageObjs.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <Config.h>

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


std::ostream& 
operator << (std::ostream& os, const PicMetaDataObj& picMetaData)
{
	os << " PicMetaDataObj appId=" << picMetaData.getAppId()  << " name=" << picMetaData.name << " frndLstStr=" << picMetaData.frndLstStr << " shrId=" << picMetaData.shrId << " pic_len=" << picMetaData.picLen << " writtenLen=" << picMetaData.writtenLen << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return os;
}

std::ostream& operator << (std::ostream& os, const LstObj& lstObj)
{
    os << " LstObj appId=" << lstObj.getAppId() << " name=" << lstObj.getName() << 
    " shareId=" << lstObj.getShrId() << " list=" << lstObj.getList() << " ";
    return os;
}

void
PicMetaDataObj::setName(const char *pName, int len)
{
	name.resize(len);
	memcpy(&name[0], pName, len);
	std::size_t fnd = name.find_first_of('\0');
	if (fnd != std::string::npos)
		name.resize(fnd);
	return;
}

std::ostream&
operator << (std::ostream& os, const StorePurchasedObj& storePurchase)
{

    os << "StorePurchasedObj appId=" << storePurchase.getAppId() << " shareId="
        << storePurchase.getShareId() << " productId=" << storePurchase.getPurchaseId() << " deviceId=" << storePurchase.getDeviceId() << " fd=" << storePurchase.getFd();
        
    return os;
}
std::ostream&
operator << (std::ostream& os, const GetRemoteHostObj& getRemoteHost)
{
    os << "GetRemoteHostObj appId=" << getRemoteHost.getAppId() << " shareId="
        << getRemoteHost.getShareId() << " " << __FILE__ << ":" << __LINE__ << std::endl;   
	return os;
}

std::ostream&
operator << (std::ostream& os, const devTknArchv& devTkInfo)
{
	os << " appId=" << devTkInfo.appId << " shareId=" << devTkInfo.shareId << " tkn_len=" << devTkInfo.tkn_len << " platform=" << devTkInfo.platform ;
	return os;
}

std::ostream& operator << (std::ostream& os, const ShouldDownLoad& shldDwld)
{
	os << " appId=" << shldDwld.getAppId() << " shareId=" << shldDwld.getShrId() << " name=" << shldDwld.getName() << " download=" << shldDwld.downLoad();
	return os;
}

std::ostream&
operator << (std::ostream& os, const shareInfo& shInfo)
{
    os << " appId=" << shInfo.appId << " shrIdLst=" << shInfo.shrIdLst << " shrId=" << shInfo.shrId << " del=" << shInfo.del << " name_len=" << shInfo.name_len << " list_len=" << shInfo.list_len;
    return os;
}

std::ostream&
operator << (std::ostream& os, const picShareInfo& shInfo)
{
	const shareInfo& base = shInfo;
	os << base << " pic_len=" << shInfo.pic_len;
	return os;
}

std::ostream&
operator << (std::ostream& os, const GetItemObj& getItem)
{
	os << " appId=" << getItem.getAppId() << " shrId=" << getItem.getShrId() << " devId=" << getItem.getDeviceId() << " fd=" << getItem.getFd() << " picName=" << getItem.getPicName() << " picRemaining=" << getItem.getPicRemaining() << " picShareId=" << getItem.getPicShareId() << " maxShareId=" << getItem.getMaxShareId();
	return os;
}

std::ostream&
operator << (std::ostream& os, const PicDoneObj& picItem)
{
	os << " appId=" << picItem.getAppId() << " shrId=" << picItem.getShrId() << " fd=" << picItem.getFd() << " picName=" << picItem.getPicName() << " picShareId=" << picItem.getPicShareId();
	return os;
}



PicMetaDataObj::PicMetaDataObj()
{
	shrId=0;
	picLen =0;
	writtenLen =0;
	std::cout << "created PicMetaDataObj " << __FILE__ << ":" << __LINE__ << std::endl;
}

std::vector<std::string>
PicMetaDataObj::getLocalFrndLst() const
{
    std::vector<std::string> lclShareIds;

    for (auto shareIdStr : frndLst)
    {
        long nShareid = std::stol(shareIdStr);
        if (nShareid >= Config::Instance().getStartShareId() && nShareid <= Config::Instance().getEndShareId())
        {
            lclShareIds.push_back(shareIdStr);
        }
    }
   
    return lclShareIds; 

}

void
PicMetaDataObj::setFrndLst(const std::vector<std::string>& fl)
{
    frndLst = fl;
    std::stringstream ss;
    for (auto& frnd : fl)
    {
        ss << frnd << ";";
    }
    frndLstStr = ss.str(); 
}

void
PicMetaDataObj::setFrndLstStr(const std::string& flist)
{
	frndLstStr = flist;
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
	std::size_t fnd = name.find_first_of('\0');
	if (fnd != std::string::npos)
		name.resize(fnd);
	return;
}

void
TemplLstObj::setTemplList(const char *pList, int len)
{
	templList.resize(len);
	memcpy(&templList[0], pList, len);
	std::size_t fnd = templList.find_first_of('\0');
	if (fnd != std::string::npos)
		templList.resize(fnd);
	return;
}

void
LstObj::setName(const char *pName, int len)
{
	name.resize(len);
	memcpy(&name[0], pName, len);
	std::size_t fnd = name.find_first_of('\0');
	if (fnd != std::string::npos)
		name.resize(fnd);
	return;
}

void
LstObj::setList(const char *pList, int len)
{
	list.resize(len);
	memcpy(&list[0], pList, len);
	std::size_t fnd = list.find_first_of('\0');
	if (fnd != std::string::npos)
		list.resize(fnd);
	return;
}
