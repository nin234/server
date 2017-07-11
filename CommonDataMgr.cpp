#include <CommonDataMgr.h>
#include <ArchiveMgr.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <sstream>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <ArchiveMsgCreator.h>
#include <Util.h>

CommonDataMgr::CommonDataMgr()
{
	
	std::shared_ptr<CommonArchvr> pCommonArch = ArchiveMgr::Instance().getCommonArchvr();
	std::cout << "Populating data cache " << __FILE__ << ":" << __LINE__ << std::endl;
	if (pCommonArch)
	{
		std::cout << "Populating Archive Items " << __FILE__ << ":" << __LINE__ << std::endl;
		pCommonArch->populateArchvItems([&](int appId, long shareId, const std::string& name, const std::string& templList){storeArchiveItem(appId, shareId, name, templList);});
		std::cout << "Populating Items " << __FILE__ << ":" << __LINE__ << std::endl;
		pCommonArch->populateItem([&](int appId, long shareId, const std::string& name, const std::string& list){storeItem(appId, shareId, name, list);});
		std::cout << "Populating device tokens " << __FILE__ << ":" << __LINE__ << std::endl;
		
		pCommonArch->populateDeviceTkn([&](int appId, long shareId, const std::string& devId, const std::string& devTkn){storeDeviceTkn(appId, shareId, devTkn, devId);}); //devId here is platform ie ios or android
		std::cout << "Populating device tokens " << __FILE__ << ":" << __LINE__ << std::endl;
        pCommonArch->populateTemplItem([&](int appId, long shareId, const std::string& name, const std::string& list){storeTemplItem(appId, shareId, name, list);});
   		std::cout << "Populating share list " << __FILE__ << ":" << __LINE__ << std::endl;
        
        pCommonArch->populateShareLst([&](int appId, long shareId, const std::string& name, long& shareIdLst){storeLstShareInfo(appId, shareId, name, shareIdLst);});
   		std::cout << "Populating template share list " << __FILE__ << ":" << __LINE__ << std::endl;
        pCommonArch->populateTemplShareLst([&](int appId, long shareId, const std::string& name, long& shareIdLst){storeTemplLstShareInfo(appId, shareId, name, shareIdLst);});
	}
	std::cout << "Populated data cache " << __FILE__ << ":" << __LINE__ << std::endl;
}

void
CommonDataMgr::storeLstShareInfo(int appId, long shareId, const std::string& name, long shareIdLst)
{
	std::cout << "Storing item shareInfo appId=" << appId << " shareId=" << shareId << " name=" << name << " shareIdLst=" << shareIdLst << " " << __FILE__ << ":" << __LINE__ << std::endl;
    
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
	std::cout << "Storing archive items " << __FILE__ << ":" << __LINE__ << std::endl;
  CommonElem& elem = commonElems[appId][shareId];
  elem.archvItems.insert(name, templList);
  return;                                
}                                              

void
CommonDataMgr::storeItem(int appId, long shareId, const std::string& name, const std::string& list)
{
	std::cout << "Storing item appId=" << appId << " shareId=" << shareId << " name=" << name << " list=" << list << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
	std::cout << "Storing device token message appId=" << appId << " shareId=" << shareId << " devTkn=" << devTkn << " platform=" << platform << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
		if (!file.size())
		{
			std::cout << "Invalid picture file shareId=" << pItr1->second->getShrId() << " appId=" << pPicObj->getAppId() << " name=" << pItr1->second->getName() << " " << __FILE__ << ":" << __LINE__ << std::endl;
			return false;
		}
		fd = open(file.c_str(), O_CREAT|O_RDWR|O_TRUNC, S_IRWXU|S_IRGRP|S_IROTH);
		std::cout << "Opened file=" << file << " to store picture " << __FILE__ << ":" << __LINE__ << std::endl;
		
		if (fd == -1)
		{
			std::cout << "Failed to open file " << file  << " " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
	std::cout << "Storing picMetaData appId=" << appId << " shareIdLst=" << shareIdLst << " name=" << name << " picLen=" << pPicMetaObj->getPicLen();
	for (const std::string& shareId : shareIds)
	{
		std::cout << " shareId=" << shareId;
  		CommonElem& elem = commonElems[appId][std::stol(shareId)];	
		LckFreeLstSL &lstSS = elem.picShareInfo[shareIdLst];
		long val = pPicMetaObj->getPicLen();
		lstSS.insertOrUpdate(name, val);
		
	}
	std::cout << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
		std::cout << "Storing item shareInfo shId=" << shId << " appId=" << appId << " shareIdLst=" << shareIdLst << " name=" << name << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
CommonDataMgr::getAndroidDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens)
{
    for (const std::string& shareId : shareIds)
    {
        CommonElem& elem = commonElems[appId][std::stol(shareId)];
        if (elem.os == "android")
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
		std::cout << "Erasing itemName=" << itemName << " shareId=" << shareId << " frndShareId=" << frndShareId << " appId=" << appId << " " << __FILE__ << ":" << __LINE__ << std::endl;
		lstSS.erase(itemName);
		if (lstSS.isEmpty())
		{
			std::cout << "lstSS cleanUp " << __FILE__ << ":" << __LINE__ << std::endl;
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
		long shrid_of_frndlst; 
		LckFreeLstSL& lstSS = elem.picShareInfo.getNext(shrid_of_frndlst, isNext, indx);
		
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
		long shrid_of_frndlst; 
		LckFreeLstSS& lstSS = elem.lstShareInfo.getNext(shrid_of_frndlst, isNext, indx);
		
		if (!isNext)
			break;
		std::vector<std::string> itemNames;
		lstSS.getKeys(itemNames);

		CommonElem& shrelem = commonElems[appId][shrid_of_frndlst];
		std::cout << "Number of keys=" << itemNames.size() << " " << __FILE__ << " " << __LINE__ << std::endl;
		for (const std::string& itemName : itemNames)
		{
			std::string item;
			if (!shrelem.items.getVal(itemName, item))
				std::cout << "Failed to get item for itemName=" << itemName << " " << __FILE__ << ":" << __LINE__ << std::endl;
			std::cout << "Getting shareList i=" << i <<" appId=" << appId << " shareId=" << shareId << " shrid_of_frndlst=" << shrid_of_frndlst << " itemName=" << itemName << " item=" << item << " " << __FILE__ << " " << __LINE__ << std::endl;
			shrIdLstName shlst;
			shlst.shareId = shrid_of_frndlst;
			shlst.lstName = itemName;
			lstNameMp[shlst] = item;
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
        long shrid_of_frndlst;
	LckFreeLstSS& lstSS = elem.templLstShareInfo.getNext(shrid_of_frndlst, isNext, indx);
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
            lstNameMp[shlst] = item;
        }
        
    }
    
}
