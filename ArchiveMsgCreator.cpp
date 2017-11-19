#include <ArchiveMsgCreator.h>
#include <string.h>
#include <Constants.h>
#include <iostream>

bool
ArchiveMsgCreator::createShareIdMsg(char *pMsg, int& len, long shareId)
{
	constexpr int msglen = sizeof(int) + sizeof(long);
	len = msglen;
	constexpr int msgId = ARCHIVE_SHARE_ID_MSG;
	memcpy(pMsg, &msgId, sizeof(int));
	memcpy(pMsg+sizeof(int), &shareId, sizeof(long));
	return true;
}

bool
ArchiveMsgCreator::createTrnIdShrIdMsg(char *pMsg, int& len, shrIdTrnId shtrId)
{
	constexpr int msglen = sizeof(int) + sizeof(shrIdTrnId);
	len = msglen;
	constexpr int msgId = ARCHIVE_SHARE_TRN_ID_MSG;
	memcpy(pMsg, &msgId, sizeof(int));
	memcpy(pMsg+sizeof(int), &shtrId, sizeof(shrIdTrnId));
	return true;
}

bool 
ArchiveMsgCreator::createDevTknMsg(char *pMsg, int& len, int appId, long shareId, const std::string& devTkn, const std::string& platform)
{

	constexpr int msgId = ARCHIVE_DEVICE_TKN_MSG;
	memcpy(pMsg, &msgId, sizeof(int));
	devTknArchv devLens;
	devLens.shareId = shareId;
	devLens.appId = appId;
	devLens.tkn_len = devTkn.size() + 1;
    if (platform == "android")
    {
        devLens.platform = 1;
    }
    else if (platform == "ios")
    {
        devLens.platform = 0;
    }
     else
     {
         devLens.platform = 100;
    }

	int msglen = sizeof(int) + sizeof(devTknArchv) + devLens.tkn_len ;

	len = msglen;	

	memcpy(pMsg+sizeof(int), &devLens, sizeof(devTknArchv));
	constexpr int devTknoffset = sizeof(int) + sizeof(devTknArchv);
	memcpy(pMsg+devTknoffset, devTkn.c_str(), devLens.tkn_len);
	std::cout << "Created archive device token msg " << devLens << " devTkn=" << devTkn << " " << __FILE__ << ":" << __LINE__ << std::endl;
	return true;
}

std::unique_ptr<char>
ArchiveMsgCreator::createFrndLstMsg(char *pMsgStatic, int& len, long shareId, const std::string& frndLst, int buflen)
{
	constexpr int shLen = sizeof(int) + sizeof(shrdIdSize);
	int msglen = shLen + frndLst.size() + 1;
	len = msglen;
	std::unique_ptr<char> pMsgDynamic;
	char *pMsg;
	if (buflen >= msglen)
		pMsg = pMsgStatic;
	else
	{
		std::unique_ptr<char> tmp{new char[msglen]};
		pMsgDynamic = std::move(tmp);
		pMsg = pMsgDynamic.get();	
		std::cout << "Dyanamic allocation in createFrndLstMsg shareId=" << shareId << " " << frndLst << std::endl;
	}
	
	constexpr int msgId = ARCHIVE_FRND_LST_MSG;
	memcpy(pMsg, &msgId, sizeof(int));
	shrdIdSize shIdSize;
	shIdSize.size = frndLst.size()+1;
	shIdSize.shrId = shareId;
	memcpy (pMsg+sizeof(int), &shIdSize, sizeof(shrdIdSize));
	memcpy (pMsg+shLen, frndLst.c_str(), frndLst.size()+1);
	return pMsgDynamic;
}

std::unique_ptr<char>
ArchiveMsgCreator::createArchvItmMsg(char *pMsgStatic, int& len, long shareId, const std::string& name, const std::string& templList, int buflen)
{
	constexpr int msgId = ARCHIVE_ARCHIVE_ITEM_MSG;
	shrdIdTemplSize templSize;
	templSize.shrId = shareId;
	templSize.name_len = name.size() + 1;
	templSize.list_len = templList.size() + 1;

	int msglen = sizeof(int) + sizeof(shrdIdTemplSize) + templSize.name_len + templSize.list_len ;

	len = msglen;	
	char *pMsg;
	std::unique_ptr<char> pMsgDynamic;
	if (buflen >= msglen)
		pMsg = pMsgStatic;
	else
	{
		std::unique_ptr<char> tmp{new char[msglen]};
		pMsgDynamic = std::move(tmp);
		pMsg = pMsgDynamic.get();	
		std::cout << "Dyanamic allocation in createArchvItmMsg shareId=" << shareId << " name="  << name << " templList=" << templList << std::endl;
	}
	memcpy(pMsg, &msgId, sizeof(int));
	memcpy(pMsg+sizeof(int), &templSize, sizeof(shrdIdTemplSize));
	constexpr int nameoffset = sizeof(int) + sizeof(shrdIdTemplSize);
	memcpy(pMsg+nameoffset, name.c_str(), templSize.name_len);
	int templListoffset = sizeof(int) + sizeof(shrdIdTemplSize) + templSize.name_len;
	memcpy(pMsg+templListoffset, templList.c_str(), templSize.list_len);

	return pMsgDynamic;
}

 bool
ArchiveMsgCreator::createCmnTemplAndItemMsg(char *pMsg, int& len, int appId, long shareId, const std::string& name, const std::string& list, int buflen, int msgId)
{
    shrdIdTemplSize templSize;
    templSize.shrId = shareId;
    templSize.name_len = name.size()+1;
    templSize.list_len = list.size()+1;
    templSize.appId = appId;
    
    int msglen = sizeof(int) + sizeof(shrdIdTemplSize) + templSize.name_len + templSize.list_len ;
    
    len = msglen;
    
    if (buflen < msglen)
    {
        std::cout << "Message len " << msglen << " greater than buflen " << buflen << std::endl;
        return false;
    }
    memcpy(pMsg, &msgId, sizeof(int));
    memcpy(pMsg+sizeof(int), &templSize, sizeof(shrdIdTemplSize));
    constexpr int nameoffset = sizeof(int) + sizeof(shrdIdTemplSize);
    memcpy(pMsg+nameoffset, name.c_str(), templSize.name_len);
    int listoffset = sizeof(int) + sizeof(shrdIdTemplSize) + templSize.name_len;
    memcpy(pMsg+listoffset, list.c_str(), templSize.list_len);
    return true;

}

bool
ArchiveMsgCreator::createTemplItemMsg(char *pMsg, int& len, int appId, long shareId, const std::string& name, const std::string& list, int buflen)
{
    constexpr int msgId = ARCHIVE_TEMPL_ITM_MSG;
    return createCmnTemplAndItemMsg(pMsg, len, appId, shareId, name, list, buflen, msgId);
}

bool
ArchiveMsgCreator::createItemMsg(char *pMsg, int& len, int appId, long shareId, const std::string& name, const std::string& list, int buflen)
{

	constexpr int msgId = ARCHIVE_ITM_MSG;
	return createCmnTemplAndItemMsg(pMsg, len, appId, shareId, name, list, buflen, msgId);

}

bool 
ArchiveMsgCreator::createPicMetaDataMsg(char *pMsg, int &len, int appId, bool del, long shareId, long shareIdLst, const std::string& name, int maxlen, int pic_len)
{
    constexpr int msgId = ARCHIVE_PIC_METADATA_MSG;

    picShareInfo templSize;
    templSize.shrId = shareId;
    templSize.shrIdLst = shareIdLst;
    templSize.name_len = name.size()+1;
    templSize.del = del;
    templSize.appId = appId;
    templSize.pic_len = pic_len; 
    int msglen = sizeof(int) + sizeof(picShareInfo) + templSize.name_len;
    if (msglen > maxlen)
    {
        std::cout << "msglen=" << msglen << " greater than maxlen=" << maxlen << " ArchiveMsgCreator::createPicMetaDataMsg failed " << std::endl;
        return false;
    }
    memcpy(pMsg, &msgId, sizeof(int));
    memcpy(pMsg+sizeof(int), &templSize, sizeof(picShareInfo));
    constexpr int nameoffset = sizeof(int) + sizeof(picShareInfo);
    memcpy(pMsg+nameoffset, name.c_str(), templSize.name_len);
    len = msglen;
    std::cout << "Archiving picMetaDataMsg " <<  templSize << " name=" << name << " "  << __FILE__ << ":" << __LINE__ << std::endl;

	return true;
}

bool
ArchiveMsgCreator::createCmnShareTemplAndLstMsg(char *pMsg, int& len, int appId, bool del,   long shareId, long shareIdLst, const std::string& name, int maxlen, int msgId)
{
    shareInfo templSize;
    templSize.shrId = shareId;
    templSize.shrIdLst = shareIdLst;
    templSize.name_len = name.size()+1;
    templSize.del = del;
    templSize.appId = appId;
    int msglen = sizeof(int) + sizeof(shareInfo) + templSize.name_len;
    if (msglen > maxlen)
    {
        std::cout << "msglen=" << msglen << " greater than maxlen=" << maxlen << " ArchiveMsgCreator::createCmnShareTemplAndLstMsg failed " << std::endl;
        return false;
    }
    memcpy(pMsg, &msgId, sizeof(int));
    memcpy(pMsg+sizeof(int), &templSize, sizeof(shareInfo));
    constexpr int nameoffset = sizeof(int) + sizeof(shareInfo);
    memcpy(pMsg+nameoffset, name.c_str(), templSize.name_len);
    len = msglen;
    std::cout << "Archiving shareLstMsg" <<  templSize << " name=" << name << " "  << __FILE__ << ":" << __LINE__ << std::endl;
    return true;

    
}

bool
ArchiveMsgCreator::createShareTemplLstMsg(char *pMsg, int& len, int appId, bool del,   long shareId, long shareIdLst, const std::string& name, int maxlen)
{
    constexpr int msgId =  ARCHIVE_SHARE_TEMPL_LST_MSG;
    return createCmnShareTemplAndLstMsg(pMsg, len, appId, del, shareId, shareIdLst, name, maxlen, msgId);
}

bool
ArchiveMsgCreator::createShareLstMsg(char *pMsg, int& len, int appId, bool del, long shareId, long shareIdLst,  const std::string& name, int maxlen)
{

	constexpr int msgId = ARCHIVE_SHARE_LST_MSG;
    return createCmnShareTemplAndLstMsg(pMsg, len, appId, del, shareId, shareIdLst, name, maxlen, msgId);
}

