#include <CommonDataMgr.h>
#include <ArchiveMgr.h>
#include <string>
#include <string.h>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>
#include <ArchiveMsgCreator.h>
#include <Util.h>

CommonDataMgr::CommonDataMgr()
{
	std::shared_ptr<CommonArchvr> pCommonArch = ArchiveMgr::Instance().getCommonArchvr();
	if (pCommonArch)
	{
		pCommonArch->populateArchvItems([&](int appId, long shareId, const std::string& name, const std::string& templList){storeArchiveItem(appId, shareId, name, templList);});
		pCommonArch->populateItem([&](int appId, long shareId, const std::string& name, const std::string& list){storeItem(appId, shareId, name, list);});
		
		pCommonArch->populateDeviceTkn([&](int appId, long shareId, const std::string& devId, const std::string& devTkn){storeDeviceTkn(appId, shareId, devTkn, devId);}); //devId here is platform ie ios or android
        pCommonArch->populateTemplItem([&](int appId, long shareId, const std::string& name, const std::string& list){storeTemplItem(appId, shareId, name, list);});
        
        pCommonArch->populateShareLst([&](int appId, long shareId, const std::string& name, long& shareIdLst){storeLstShareInfo(appId, shareId, name, shareIdLst);});
        pCommonArch->populateTemplShareLst([&](int appId, long shareId, const std::string& name, long& shareIdLst){storeTemplLstShareInfo(appId, shareId, name, shareIdLst);});
	}
}

void
CommonDataMgr::storeLstShareInfo(int appId, long shareId, const std::string& name, long shareIdLst)
{
    
  		CommonElem& elem = commonElems[appId][shareId];
    LckFreeLstSS &lstSS = elem.lstShareInfo[shareIdLst];
    std::string val = "NONE";
    lstSS.insertOrUpdate(name, val);
}

void
CommonDataMgr::storeTemplLstShareInfo(int appId, long shareId, const std::string& name, long shareIdLst)
{
    CommonElem& elem = commonElems[appId][shareId];
    LckFreeLstSS &lstSS = elem.templLstShareInfo[shareIdLst];
    std::string val = "NONE";
    lstSS.insertOrUpdate(name, val);
}

CommonDataMgr::~CommonDataMgr()
{

}

void
CommonDataMgr::storeArchiveItem(int appId, long shareId, const std::string& name, const std::string& templList)
{
  CommonElem& elem = commonElems[appId][shareId];
  elem.archvItems.insert(name, templList);
  return;                                
}                                              

void
CommonDataMgr::storeItem(int appId, long shareId, const std::string& name, const std::string& list)
{
  	CommonElem& elem = commonElems[appId][shareId];
  	elem.items.insert(name, list);
	return;
}

void
CommonDataMgr::storeTemplItem(int appId, long shareId, const std::string& name, const std::string& list)
{
    CommonElem& elem = commonElems[appId][shareId];
    elem.templItems.insert(name, list);
    return;
}



void
CommonDataMgr::storeDeviceTkn(int appId, long shareId, const std::string& devTkn, const std::string& platform)
{
  	CommonElem& elem = commonElems[appId][shareId];
  	elem.deviceToken = devTkn;
    elem.os = platform;
	return;
}

std::string 
CommonDataMgr::getPicName(int fd) 
{

	auto pItr = fdPicMetaMp.find(fd);
	if (pItr != fdPicMetaMp.end())
	{
		return pItr->second->getName();
	}
	std::string noName;
	return noName;
}

std::vector<std::string> 
CommonDataMgr::getPicShareIds(int fd)
{

	auto pItr = fdPicMetaMp.find(fd);
	if (pItr != fdPicMetaMp.end())
	{
		return pItr->second->getFrndLst();
	}
	std::vector<std::string> emptyVec;
	return emptyVec;
}

bool
CommonDataMgr::storePic(PicObj *pPicObj)
{
	auto pItr = fdFdMp.find(pPicObj->getFd());
	auto pItr1 = fdPicMetaMp.find(pPicObj->getFd());
	if (pItr1 == fdPicMetaMp.end())
	{
		std::cout << "Failed to find metadata for picture " << std::endl;
		return false;
	}
	int fd  = -1;
	if (pItr != fdFdMp.end())
	{	
		fd = pItr->second;	
	}		
	else
	{
		std::string file = Util::constructPicFile(pItr1->second->getShrId(), pPicObj->getAppId(), pItr1->second->getName());
		fd = open(file.c_str(), O_CREAT|O_RDWR);
		
		if (fd == -1)
		{
			std::cout << "Failed to open file " << file << std::endl;
			fdPicMetaMp.erase(fd);
			return false;
		}
		fdFdMp[pPicObj->getFd()] = fd;
	}
	if (write(fd, pPicObj->getBuf() + 2*sizeof(int), pPicObj->getBufLen() - 2*sizeof(int)) == -1)
	{
		fdFdMp.erase(pPicObj->getFd());
		fdPicMetaMp.erase(fd);
		std::cout << "Failed to store picture " << pItr1->second->getName() << std::endl;
		return false;
	}
	int lenWrittenSoFar = pItr1->second->getWrittenLen();
	int lenWrittenNow = pPicObj->getBufLen() - 2*sizeof(int);
	int totLenWritten = lenWrittenSoFar + lenWrittenNow;
	if (totLenWritten >= pItr1->second->getPicLen())
	{
		std::cout << "Picture " << pItr1->second->getName() << " stored picLen=" << pItr1->second->getPicLen() << " totalWritten=" << totLenWritten << std::endl;
		close(fd);
		fdFdMp.erase(fd);
		fdPicMetaMp.erase(fd);
		//signifies that the picture is completly stored , time to send push notification
		return true;
	}
	pItr1->second->setWrittenLen(totLenWritten);
	//not an error condition, signifies that more data expected
	return false;

}
void 
CommonDataMgr::eraseFdMp(int fd)
{
	fdFdMp.erase(fd);
	fdPicMetaMp.erase(fd);
}
void 
CommonDataMgr::storePicMetaData(PicMetaDataObj *pPicMetaObj)
{
	int appId = pPicMetaObj->getAppId();
	long shareIdLst = pPicMetaObj->getShrId();
	std::string name = pPicMetaObj->getName();
	const std::vector<std::string>& shareIds = pPicMetaObj->getFrndLst();
	for (const std::string& shareId : shareIds)
	{
  		CommonElem& elem = commonElems[appId][std::stol(shareId)];	
		LckFreeLstSL &lstSS = elem.picShareInfo[shareIdLst];
		long val = pPicMetaObj->getPicLen();
		lstSS.insertOrUpdate(name, val);
	}
	std::unique_ptr<PicMetaDataObj> pPicMetaUPtr(pPicMetaObj);
	fdPicMetaMp[pPicMetaObj->getFd()] = std::move(pPicMetaUPtr);
	return;
}

void
CommonDataMgr::storeTemplLstShareInfo(int appId, long shareIdLst, const std::vector<std::string>& shareIds, const std::string& name)
{
    for (const std::string& shareId : shareIds)
    {
        long shId = std::stol(shareId);
        CommonElem& elem = commonElems[appId][shId];
        LckFreeLstSS &lstSS = elem.templLstShareInfo[shareIdLst];
        std::string val = "NONE";
        lstSS.insertOrUpdate(name, val);
    }	
    return;
}


void
CommonDataMgr::storeLstShareInfo(int appId, long shareIdLst, const std::vector<std::string>& shareIds, const std::string& name)
{
	for (const std::string& shareId : shareIds)
	{
		long shId = std::stol(shareId);
  		CommonElem& elem = commonElems[appId][shId];
		LckFreeLstSS &lstSS = elem.lstShareInfo[shareIdLst];
		std::string val = "NONE";
		lstSS.insertOrUpdate(name, val);
	}	
	return;
}




CommonDataMgr&
CommonDataMgr::Instance()
{
	static CommonDataMgr instance;
	return instance;
}

void
CommonDataMgr::getDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens)
{
	for (const std::string& shareId : shareIds)
	{
  		CommonElem& elem = commonElems[appId][std::stol(shareId)];
        if (elem.os == "ios")
            tokens.push_back(elem.deviceToken);
	}
	return;
}

void
CommonDataMgr::updateLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName)
{
		
	CommonElem& elem = commonElems[appId][shareId];
	LckFreeLstSS lstSS;
	if (elem.lstShareInfo.getValue(frndShareId, lstSS))
	{
		lstSS.erase(itemName);
		if (lstSS.isEmpty())
		{
			lstSS.cleanUp();
		}
	}
	return;
}

void
CommonDataMgr::updateTemplLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName)
{
    CommonElem& elem = commonElems[appId][shareId];
    LckFreeLstSS lstSS;
    if (elem.templLstShareInfo.getValue(frndShareId, lstSS))
    {
        lstSS.erase(itemName);
        if (lstSS.isEmpty())
        {
            lstSS.cleanUp();
        }
    }
    return;
}

void
CommonDataMgr::getPictureNames(int appId, long shareId, std::vector<shrIdLstName>& picNamesShIds)
{
	
  	CommonElem& elem = commonElems[appId][shareId];
	bool isNext = true;
	int indx = -1;
	for (int i=0; i < SHARE_MAP_SIZE; ++i)
	{
		LckFreeLstSL lstSS;
		long shrid_of_frndlst; 
		isNext = elem.picShareInfo.getNext(shrid_of_frndlst, lstSS, indx);
		
		if (!isNext)
			break;
		std::map<std::string, long> picNamesLens;
		lstSS.getKeyVals(picNamesLens);
		for (auto pItr = picNamesLens.begin(); pItr != picNamesLens.end(); ++pItr)
		{
			shrIdLstName shlst;
			shlst.shareId = shrid_of_frndlst;
			shlst.lstName = pItr->first;
			shlst.picLen =  pItr->second;
			shlst.appId = appId;
			picNamesShIds.push_back(shlst);

		}

	}
	return;
}

void
CommonDataMgr::getShareLists(int appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp)
{
	CommonElem& elem = commonElems[appId][shareId];
	bool isNext = true;
	int indx = -1;
	for (int i=0; i < SHARE_MAP_SIZE; ++i)
	{
		LckFreeLstSS lstSS;
		long shrid_of_frndlst; 
		isNext = elem.lstShareInfo.getNext(shrid_of_frndlst, lstSS, indx);
		
		if (!isNext)
			break;
		std::vector<std::string> itemNames;
		lstSS.getKeys(itemNames);

		CommonElem& shrelem = commonElems[appId][shrid_of_frndlst];
		for (const std::string& itemName : itemNames)
		{
			std::string item;
			shrelem.items.getVal(itemName, item);
			shrIdLstName shlst;
			shlst.shareId = shrid_of_frndlst;
			shlst.lstName = itemName;
			lstNameMp[shlst] = item.substr(2*sizeof(long));
		}
	}
	return;
}

void
CommonDataMgr::getShareTemplLists(int appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp)
{
    CommonElem& elem = commonElems[appId][shareId];
    bool isNext = true;
    int indx = -1;
    for (int i=0; i < SHARE_MAP_SIZE; ++i)
    {
        LckFreeLstSS lstSS;
        long shrid_of_frndlst;
        isNext = elem.templLstShareInfo.getNext(shrid_of_frndlst, lstSS, indx);
        if (!isNext)
            break;
        std::vector<std::string> itemNames;
        lstSS.getKeys(itemNames);
        CommonElem& shrelem = commonElems[appId][shrid_of_frndlst];
        for (const std::string& itemName : itemNames)
        {
            std::string item;
            shrelem.templItems.getVal(itemName, item);
            shrIdLstName shlst;
            shlst.shareId = shrid_of_frndlst;
            shlst.lstName = itemName;
            lstNameMp[shlst] = item.substr(2*sizeof(long));
        }
        
    }
    
}
