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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Util.h>
#include <Config.h>
#include <FrndLstMgr.h>

thread_local std::unordered_map<int, int> 
CommonDataMgr::fdFdMp;

thread_local std::unordered_map<int, std::shared_ptr<PicMetaDataObj>> CommonDataMgr::fdPicMetaMp;

CommonDataMgr::CommonDataMgr()
{
	
	std::shared_ptr<CommonArchvr> pCommonArch = ArchiveMgr::Instance().getCommonArchvr();
	std::cout << "Populating data cache " << __FILE__ << ":" << __LINE__ << std::endl;
	if (pCommonArch)
	{
		std::cout << "Populating Archive Items " << __FILE__ << ":" << __LINE__ << std::endl;
		pCommonArch->populateArchvItems([&](int appId, long shareId, const std::string& name, const std::string& templList){storeArchiveItem(appId, shareId, name, templList);});
        
        if (!Config::Instance().useDB())
        {
            std::cout << "Populating Items " << __FILE__ << ":" << __LINE__ << std::endl;
            pCommonArch->populateItem([&](int appId, long shareId, const std::string& name, const std::string& list){storeItem(appId, shareId, name, list);});
            
            std::cout << "Populating device tokens " << __FILE__ << ":" << __LINE__ << std::endl;
            
            pCommonArch->populateDeviceTkn([&](int appId, long shareId, const std::string& devId, const std::string& devTkn){storeDeviceTkn(appId, shareId, devTkn, devId);}); //devId here is platform ie ios or android
        }
		std::cout << "Populating Templ Items " << __FILE__ << ":" << __LINE__ << std::endl;
        pCommonArch->populateTemplItem([&](int appId, long shareId, const std::string& name, const std::string& list){storeTemplItem(appId, shareId, name, list);});
   		std::cout << "Populating share list " << __FILE__ << ":" << __LINE__ << std::endl;
        
        pCommonArch->populateShareLst([&](int appId, long shareId, const std::string& name, long& shareIdLst){storeLstShareInfo(appId, shareId, name, shareIdLst);});
        
	    if (!Config::Instance().useDB())
        {
   		    std::cout << "Populating pic meta list " << __FILE__ << ":" << __LINE__ << std::endl;
            pCommonArch->populatePicMetaLst([&](int appId, long shareId, const std::string& name, long& shareIdLst, int pic_len){storePicMetaInfo(appId, shareId, name, shareIdLst, pic_len);});
        }
   		std::cout << "Populating template share list " << __FILE__ << ":" << __LINE__ << std::endl;
        pCommonArch->populateTemplShareLst([&](int appId, long shareId, const std::string& name, long& shareIdLst){storeTemplLstShareInfo(appId, shareId, name, shareIdLst);});
	}
	std::cout << "Populated data cache " << __FILE__ << ":" << __LINE__ << std::endl;
}

void
CommonDataMgr::storeLstShareInfo(int appId, long shareId, const std::string& name, long shareIdLst)
{
	std::cout << "Storing item shareInfo appId=" << appId << " shareId=" << shareId << " name=" << name << " shareIdLst=" << shareIdLst << " " << __FILE__ << ":" << __LINE__ << std::endl;
   	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
	CommonElem& elem = commonElems[appId][shareId];
	std::string val = "NONE";
	elem.lstShareInsert(shareIdLst, name, val);
}

void
CommonDataMgr::storePicMetaInfo(int appId, long shareId, const std::string& name, long shareIdLst, int pic_len)
{
	std::cout << "Storing item picMetaInfo appId=" << appId << " shareId=" << shareId << " name=" << name << " shareIdLst=" << shareIdLst << " pic_len=" << pic_len << " " << __FILE__ << ":" << __LINE__ << std::endl;
   	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
	CommonElem& elem = commonElems[appId][shareId];
	elem.picShareInsert(shareIdLst, name, pic_len);
}


void
CommonDataMgr::storeTemplLstShareInfo(int appId, long shareId, const std::string& name, long shareIdLst)
{
    std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
    CommonElem& elem = commonElems[appId][shareId];
    std::string val = "NONE";
    elem.templLstShareInsert(shareIdLst, name, val);
}

CommonDataMgr::~CommonDataMgr()
{

}

void
CommonDataMgr::storeArchiveItem(int appId, long shareId, const std::string& name, const std::string& templList)
{
	std::cout << "Storing archive items " << __FILE__ << ":" << __LINE__ << std::endl;
    std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
  CommonElem& elem = commonElems[appId][shareId];
  elem.archvItems.insert(name, templList);
  return;                                
}                                              

void
CommonDataMgr::storeItem(int appId, long shareId, const std::string& name, const std::string& list)
{
	std::cout << "Storing item appId=" << appId << " shareId=" << shareId << " name=" << name << " list=" << list << " " << __FILE__ << ":" << __LINE__ << std::endl;
    std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
  	CommonElem& elem = commonElems[appId][shareId];
  	elem.items.insert(name, list);
	return;
}

void
CommonDataMgr::storeTemplItem(int appId, long shareId, const std::string& name, const std::string& list)
{
    std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
    CommonElem& elem = commonElems[appId][shareId];
    elem.templItems.insert(name, list);
    return;
}



void
CommonDataMgr::storeDeviceTkn(int appId, long shareId, const std::string& devTkn, const std::string& platform)
{
	if (!Config::Instance().useDB())
	{

		std::cout << Util::now() << "Storing device token message appId=" << appId << " shareId=" << shareId << " devTkn=" << devTkn << " platform=" << platform << " " << __FILE__ << ":" << __LINE__ << std::endl;
	    std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
		CommonElem& elem = commonElems[appId][shareId];
		elem.deviceToken = devTkn;
		elem.os = platform;
	}

	if (Config::Instance().useDB())
	{
		m_rocksDAO.storeDeviceTkn(appId, shareId, devTkn, platform);
	}
	return;
}

std::shared_ptr<PicMetaDataObj>
CommonDataMgr::getPicMetaObj(int fd)
{

	auto pItr = fdPicMetaMp.find(fd);
	if (pItr != fdPicMetaMp.end())
	{
		return pItr->second;
	}
    return std::shared_ptr<PicMetaDataObj>();    
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
CommonDataMgr::shouldUpload(std::shared_ptr<PicMetaDataObj> pPicMetaObj, int *picOffset)
{
	*picOffset = 0;
	std::string file = Util::constructPicFile(pPicMetaObj->getShrId(), pPicMetaObj->getAppId(), pPicMetaObj->getName());
	if (!file.size())
	{
		std::cout << "Invalid picture file shareId=" << pPicMetaObj->getShrId() << " appId=" << pPicMetaObj->getAppId() << " name=" << pPicMetaObj->getName() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}
	struct stat buf;
	if (stat(file.c_str(), &buf) == 0)
	{
		if (pPicMetaObj->getPicLen() > buf.st_size)
		{
			std::cout << "Need to upload picture" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
			*picOffset = buf.st_size;
			return true;
		}
		std::cout << "No need to upload picture" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		return false;
	}
	std::cout << "Need to upload picture" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return true;
}

int
CommonDataMgr::openPicFile(long shareId, int appId, const std::string& picName, std::shared_ptr<PicObj> pPicObj)
{
	int fd  = -1;
	std::string file = Util::constructPicFile(shareId, appId, picName);
	if (!file.size())
	{
		std::cout << "Invalid picture file shareId=" << shareId << " appId=" <<  appId<< " name=" <<  picName << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return fd;
	}
	fd = open(file.c_str(), O_CREAT|O_RDWR|O_TRUNC, S_IRWXU|S_IRGRP|S_IROTH);
	
	if (fd == -1)
	{
		std::cout << "Failed to open file " << file  << " " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << std::endl;
		fdPicMetaMp.erase(fd);
		return fd;
	}
	std::cout << "Opened file=" << file << " to store picture " << __FILE__ << ":" << __LINE__ << std::endl;
	fdFdMp[pPicObj->getFd()] = fd;
	return fd;

}

bool
CommonDataMgr::storePic(std::shared_ptr<PicObj> pPicObj, bool& cleanUpNtwFd)
{
    cleanUpNtwFd = false;
	auto pItr = fdFdMp.find(pPicObj->getFd());
	auto pItr1 = fdPicMetaMp.find(pPicObj->getFd());
	if (pItr1 == fdPicMetaMp.end())
	{
		std::cout << "Failed to find metadata for picture fd=" << pPicObj->getFd() << " " << __FILE__ << ":" << __LINE__ << std::endl;
        cleanUpNtwFd = true;
		return false;
	}
	int fd  = -1;
	if (pItr != fdFdMp.end())
	{	
		fd = pItr->second;	
	}		
	else
	{
		fd = openPicFile(pItr1->second->getShrId(), pPicObj->getAppId(), pItr1->second->getName(), pPicObj);
		if (fd == -1)
		{
            cleanUpNtwFd = true;
			return false;
		}
	}
	if (write(fd, pPicObj->getBuf(), pPicObj->getBufLen()) == -1)
	{
		fdFdMp.erase(pPicObj->getFd());
		fdPicMetaMp.erase(pPicObj->getFd());
		std::cout << "Failed to store picture " << pItr1->second->getName() << std::endl;
        cleanUpNtwFd = true;
		return false;
	}
	int lenWrittenSoFar = pItr1->second->getWrittenLen();
	int lenWrittenNow = pPicObj->getBufLen() ;
	int totLenWritten = lenWrittenSoFar + lenWrittenNow;
	std::cout << "Storing picture lenWrittenSoFar=" << lenWrittenSoFar << " lenWrittenNow=" << lenWrittenNow << " totLenWritten=" << totLenWritten << " pic len=" << pItr1->second->getPicLen() << " " << __FILE__ << ":" << __LINE__ << std::endl;
	if (totLenWritten >= pItr1->second->getPicLen())
	{
		std::cout << "Picture " << pItr1->second->getName() << " stored picLen=" << pItr1->second->getPicLen() << " totalWritten=" << totLenWritten << std::endl;
		storePicMetaData(pItr1->second);
		close(fd);
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
	int cnt = fdFdMp.erase(fd);
	int cnt1 = fdPicMetaMp.erase(fd);
	std::cout << "Erased from fdFdMp=" << cnt << " and fdPicMetaMp=" << cnt1 << " fd=" << fd << " " << __FILE__ << ":" << __LINE__ << std::endl;
}

void 
CommonDataMgr::storePicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
	int appId = pPicMetaObj->getAppId();
	long shareIdLst = pPicMetaObj->getShrId();
	std::string name = pPicMetaObj->getName();
	std::vector<std::string> shareIds = pPicMetaObj->getLocalFrndLst();
	std::cout << "Storing picMetaData appId=" << appId << " shareIdLst=" << shareIdLst << " name=" << name << " picLen=" << pPicMetaObj->getPicLen();
    long val = pPicMetaObj->getPicLen();
    m_picMetaDAO.store(appId, shareIdLst, name, val, shareIds);

}

bool 
CommonDataMgr::storePicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj, int *picOffset)
{
	if (shouldUpload(pPicMetaObj, picOffset))
	{
		std::cout << " fd=" << pPicMetaObj->getFd() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		fdPicMetaMp[pPicMetaObj->getFd()] = pPicMetaObj;
		return true;
	}
	else
	{
		storePicMetaData(pPicMetaObj);
	}
	return false;
}

void
CommonDataMgr::storeTemplLstShareInfo(int appId, long shareIdLst, const std::vector<std::string>& shareIds, const std::string& name)
{
    for (const std::string& shareId : shareIds)
    {
        long shId = std::stol(shareId);
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shId]); 
	CommonElem& elem = commonElems[appId][shId];
	std::string val = "NONE";
	elem.templLstShareInsert(shareIdLst, name, val);
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
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shId]); 
  		CommonElem& elem = commonElems[appId][shId];
		std::string val = "NONE";
		elem.lstShareInsert(shareIdLst, name, val);
	}	
	return;
}



void 
CommonDataMgr::storeItemAndLstShareInfo(int appId, long shareIdLst, const std::string& name, const std::string& list, const std::vector<std::string>& shareIds)
{
    m_shareItemDAO.storeItem(appId, shareIdLst, name, list, shareIds);
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
    if (Config::Instance().useDB())
	{
		m_rocksDAO.getDeviceTkns(appId, shareIds, tokens);
        return;
	}
	for (const std::string& shareId : shareIds)
	{
    		std::lock_guard<std::mutex> lock(commonElemsMtx[appId][std::stol(shareId)]); 
  		CommonElem& elem = commonElems[appId][std::stol(shareId)];
        if (elem.os == "ios")
            tokens.push_back(elem.deviceToken);
	}
	
	return;
}

void
CommonDataMgr::getAndroidDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens)
{
	if (!Config::Instance().useDB())
	{
        for (const std::string& shareId : shareIds)
        {
            std::lock_guard<std::mutex> lock(commonElemsMtx[appId][std::stol(shareId)]); 
            CommonElem& elem = commonElems[appId][std::stol(shareId)];
            if (elem.os == "android")
                tokens.push_back(elem.deviceToken);
        }
    }
    if (Config::Instance().useDB())
    {
	    m_rocksDAO.getAndroidDeviceTkns(appId, shareIds, tokens);
    }
    return;
}

void
CommonDataMgr::updateLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName)
{
		
	std::cout << "Erasing itemName=" << itemName << " shareId=" << shareId << " frndShareId=" << frndShareId << " appId=" << appId << " " << __FILE__ << ":" << __LINE__ << std::endl;
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
	CommonElem& elem = commonElems[appId][shareId];
	elem.lstShareDel(frndShareId, itemName);
	return;
}

void
CommonDataMgr::updatePicShareStatus(int appId, long shareId, long frndShareId, const std::string& picName)
{
	std::cout << "Updating ready to download picName=" << picName << " shareId=" << shareId << " frndShareId=" << frndShareId << " appId=" << appId << " " << __FILE__ << ":" << __LINE__ << std::endl;
    if (Config::Instance().useDB())
    {
        m_picMetaDAO.updateStatus(appId, frndShareId, picName, shareId);
        return;
    }
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
	CommonElem& elem = commonElems[appId][shareId];
    elem.picShareSetDwldFalse(frndShareId, picName);

}

void
CommonDataMgr::updatePicShareInfo(int appId, long shareId, long frndShareId, const std::string& picName)
{
		
	std::cout << "Erasing picName=" << picName << " shareId=" << shareId << " frndShareId=" << frndShareId << " appId=" << appId << " " << __FILE__ << ":" << __LINE__ << std::endl;
    if (Config::Instance().useDB())
    {
        m_picMetaDAO.del(appId, frndShareId, picName, shareId);
        return;
    }
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
	CommonElem& elem = commonElems[appId][shareId];
	elem.picShareDel(frndShareId, picName);
	return;
}

void
CommonDataMgr::updateTemplLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName)
{
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
    CommonElem& elem = commonElems[appId][shareId];
    elem.templLstShareDel(frndShareId, itemName);
    return;
}

void
CommonDataMgr::getPictureNames(int appId, long shareId, std::vector<shrIdLstName>& picNamesShIds)
{
	
	if (Config::Instance().useDB())
	{
        m_picMetaDAO.get(appId, shareId, picNamesShIds);
        return;
    }
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
  	CommonElem& elem = commonElems[appId][shareId];
	std::map<long, std::map<std::string, long>> shIdItemNames;
	elem.getSharePics(shIdItemNames);
	for (auto pItr = shIdItemNames.begin(); pItr != shIdItemNames.end(); ++pItr)
	{
		long shrid_of_frndlst = pItr->first; 
		std::map<std::string, long>& picNamesLens = pItr->second;
		for (auto pItr1 = picNamesLens.begin(); pItr1 != picNamesLens.end(); ++pItr1)
		{
			shrIdLstName shlst;
			shlst.shareId = shrid_of_frndlst;
			shlst.lstName = pItr1->first;
			shlst.picLen =  pItr1->second;
			shlst.appId = appId;
			shlst.shareIdElem = shareId;
			picNamesShIds.push_back(shlst);

		}

	}
	return;
}

void
CommonDataMgr::delShareLists(int appId, long shareId)
{
    m_shareItemDAO.delShareLists(appId, shareId);
}

void
CommonDataMgr::getShareLists(int appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp)
{
	if (Config::Instance().useDB())
	{
        m_shareItemDAO.getShareLists(appId, shareId, lstNameMp);
        return;
    }
    	std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
	CommonElem& elem = commonElems[appId][shareId];
	std::map<long, std::vector<std::string>> shIdItemNames;
	elem.getShareLists(shIdItemNames);
	for (auto pItr = shIdItemNames.begin(); pItr != shIdItemNames.end(); ++pItr)
	{
		long shrid_of_frndlst = pItr->first; 
		std::vector<std::string>& itemNames= pItr->second;

    	     std::lock_guard<std::mutex> lock1(commonElemsMtx[appId][shrid_of_frndlst]); 
		CommonElem& shrelem = commonElems[appId][shrid_of_frndlst];
		std::cout << "Number of keys=" << itemNames.size() << " " << __FILE__ << " " << __LINE__ << std::endl;
		for (const std::string& itemName : itemNames)
		{
			std::string item;
			if (!shrelem.items.getVal(itemName, item))
				std::cout << "Failed to get item for itemName=" << itemName << " " << __FILE__ << ":" << __LINE__ << std::endl;
			std::cout << "Getting shareList "  <<" appId=" << appId << " shareId=" << shareId << " shrid_of_frndlst=" << shrid_of_frndlst << " itemName=" << itemName << " item=" << item << " " << __FILE__ << " " << __LINE__ << std::endl;
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
   std::lock_guard<std::mutex> lock(commonElemsMtx[appId][shareId]); 
    CommonElem& elem = commonElems[appId][shareId];
    std::map<long, std::vector<std::string>> shIdItemNames;
    elem.getTemplShareLists(shIdItemNames);
    for (auto pItr = shIdItemNames.begin(); pItr != shIdItemNames.end(); ++pItr)
    {
	long shrid_of_frndlst = pItr->first; 
	std::vector<std::string>& itemNames= pItr->second;
    	     std::lock_guard<std::mutex> lock1(commonElemsMtx[appId][shrid_of_frndlst]); 
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

void
CommonDataMgr::storeFrndLst(std::shared_ptr<FrndLstObj> pFrndObj)
{
    m_frndLstMgr.storeFrndLst(pFrndObj);
}

std::string
CommonDataMgr::getFrndList(int appId, long shareId, bool bDontCheckUpdFlag)
{
    return m_frndLstMgr.getFrndList(appId, shareId, bDontCheckUpdFlag);
}

void
CommonDataMgr::updateFrndLstStatus(long shareId, int appId)
{
    m_frndLstMgr.updateStatus(shareId, appId);
}

void
CommonDataMgr::storeStorePurchase(std::shared_ptr<StorePurchasedObj> pStorePurchaseMsg)
{
    m_purchaseDAO.store(pStorePurchaseMsg->getAppId(), pStorePurchaseMsg->getShareId(), pStorePurchaseMsg->getDeviceId(), pStorePurchaseMsg->getPurchaseId());
}

bool
CommonDataMgr::getPurchase(std::shared_ptr<StorePurchasedObj> pStorePurchaseMsg)
{
    std::string purchaseId;
    if (m_purchaseDAO.get(pStorePurchaseMsg->getAppId(), pStorePurchaseMsg->getShareId(), pStorePurchaseMsg->getDeviceId(), purchaseId))
    {
        pStorePurchaseMsg->setPurchaseId(purchaseId.c_str());
    }
    else
    {
        pStorePurchaseMsg->setPurchaseId("INVALID");
    }
    return true;
}
