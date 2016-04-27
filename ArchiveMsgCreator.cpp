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
ArchiveMsgCreator::createPicMetaDataMsg(char *pMsg, int &len, long shareId, const std::string& name, const std::string& frndLst)
{
	constexpr int msgId = ARCHIVE_PIC_METADATA_MSG;
	memcpy(pMsg, &msgId, sizeof(int));
	shrdIdTemplSize frndLens;
	frndLens.shrId = shareId;
	frndLens.name_len = name.size() + 1;
	frndLens.list_len = frndLst.size() + 1;

	int msglen = sizeof(int) + sizeof(shrdIdTemplSize) + frndLens.name_len + frndLens.list_len ;

	len = msglen;	

	memcpy(pMsg+sizeof(int), &frndLens, sizeof(shrdIdTemplSize));
	constexpr int nameoffset = sizeof(int) + sizeof(shrdIdTemplSize);
	memcpy(pMsg+nameoffset, name.c_str(), frndLens.name_len);
	int frndlstoffset = sizeof(int) + sizeof(shrdIdTemplSize) + frndLens.name_len;
	memcpy(pMsg+frndlstoffset, frndLst.c_str(), frndLens.list_len);


	return true;
}

bool 
ArchiveMsgCreator::createDevTknMsg(char *pMsg, int& len, long shareId, const std::string& devId, const std::string& devTkn)
{

	constexpr int msgId = ARCHIVE_DEVICE_TKN_MSG;
	memcpy(pMsg, &msgId, sizeof(int));
	//reusing (hijacking) shrdIdTemplSize
	shrdIdTemplSize devLens;
	devLens.shrId = shareId;
	devLens.name_len = devId.size() + 1;
	devLens.list_len = devTkn.size() + 1;

	int msglen = sizeof(int) + sizeof(shrdIdTemplSize) + devLens.name_len + devLens.list_len ;

	len = msglen;	

	memcpy(pMsg+sizeof(int), &devLens, sizeof(shrdIdTemplSize));
	constexpr int devIdoffset = sizeof(int) + sizeof(shrdIdTemplSize);
	memcpy(pMsg+devIdoffset, devId.c_str(), devLens.name_len);
	int devTknoffset = sizeof(int) + sizeof(shrdIdTemplSize) + devLens.name_len;
	memcpy(pMsg+devTknoffset, devTkn.c_str(), devLens.list_len);

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
	shIdSize.size = frndLst.size();
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


std::unique_ptr<char>
ArchiveMsgCreator::createItemMsg(char *pMsgStatic, int& len, long shareId, const std::string& name, const std::string& list, int buflen)
{

	constexpr int msgId = ARCHIVE_ITM_MSG;
	shrdIdTemplSize templSize;
	templSize.shrId = shareId;
	templSize.name_len = name.size()+1;
	templSize.list_len = list.size()+1;

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
		std::cout << "Dyanamic allocation in createArchvItmMsg shareId=" << shareId << " name="  << name << " list=" << list << std::endl;
	}
	memcpy(pMsg, &msgId, sizeof(int));
	memcpy(pMsg+sizeof(int), &templSize, sizeof(shrdIdTemplSize));
	constexpr int nameoffset = sizeof(int) + sizeof(shrdIdTemplSize);
	memcpy(pMsg+nameoffset, name.c_str(), templSize.name_len);
	int listoffset = sizeof(int) + sizeof(shrdIdTemplSize) + templSize.name_len;
	memcpy(pMsg+listoffset, list.c_str(), templSize.list_len);

	return pMsgDynamic;

}

bool
ArchiveMsgCreator::createShareLstMsg(char *pMsg, int& len, long shareId, const std::string& name, const std::string& val, int maxlen)
{

	constexpr int msgId = ARCHIVE_SHARE_LST_MSG;
	shrdIdTemplSize templSize;
	templSize.shrId = shareId;
	templSize.name_len = name.size()+1;
	templSize.list_len = val.size()+1;

	int msglen = sizeof(int) + sizeof(shrdIdTemplSize) + templSize.name_len + templSize.list_len ;
	if (msglen > maxlen)
	{
		std::cout << "msglen=" << msglen << " greater than maxlen=" << maxlen << " ArchiveMsgCreator::createShareLstMsg failed " << std::endl;
		return false;
	}
	memcpy(pMsg, &msgId, sizeof(int));
	memcpy(pMsg+sizeof(int), &templSize, sizeof(shrdIdTemplSize));
	constexpr int nameoffset = sizeof(int) + sizeof(shrdIdTemplSize);
	memcpy(pMsg+nameoffset, name.c_str(), templSize.name_len);
	int listoffset = sizeof(int) + sizeof(shrdIdTemplSize) + templSize.name_len;
	memcpy(pMsg+listoffset, val.c_str(), templSize.list_len);
	len = msglen;

	return true;
}

