#include <CommonDataMgr.h>
#include <ArchiveMgr.h>
#include <string>
#include <string.h>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>

CommonDataMgr::CommonDataMgr()
{
	std::shared_ptr<CommonArchvr> pCommonArch = ArchiveMgr::Instance().getCommonArchvr();
	if (pCommonArch)
	{
		pCommonArch->populateArchvItems([&](int appId, long shareId, const std::string& name, const std::string& templList){storeArchiveItem(appId, shareId, name, templList);});
		pCommonArch->populateItem([&](int appId, long shareId, const std::string& name, const std::string& list){storeItem(appId, shareId, name, list);});
		pCommonArch->populateShareLst([&](int appId, long shareId, const std::string& name, const std::string& list){storeLstShareInfo(appId, shareId, name, list);});
		pCommonArch->populateDeviceTkn([&](int appId, long shareId, const std::string& devId, const std::string& devTkn){storeDeviceTkn(appId, shareId, devId, devTkn);});
	}
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
	constexpr int size = 2*sizeof(long);
	char arr[size];
	struct timeval now;
	gettimeofday(&now, NULL);
	long sec = now.tv_sec;
	long usec = now.tv_usec;
	memcpy(arr, &sec, sizeof(long));
	memcpy(arr+sizeof(long), &usec, sizeof(long));
	std::string listandtime = arr;
	listandtime += list;
  	CommonElem& elem = commonElems[appId][shareId];
  	elem.items.insert(name, listandtime);
	return;
}

void
CommonDataMgr::storeLstShareInfo(int appId, long shareId, const std::string& name, const std::string& list)
{
/*
  	CommonElem& elem = commonElems[appId][shareId];
  	elem.lstShareInfo.insert(name, list);
*/
	return;
}


void
CommonDataMgr::storeDeviceTkn(int appId, long shareId, const std::string& devId, const std::string& devTkn)
{
  	CommonElem& elem = commonElems[appId][shareId];
  	elem.deviceTokens.insert(devId, devTkn);
	return;
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
		std::string file = "/home/ninan/data/pictures/";
		int dir = pItr1->second->getShrId()%1000;
		std::string dirstr = std::to_string(dir);
		file += dirstr;
		file += "/";
		std::string filesuffx = std::to_string(pItr1->second->getShrId());
		filesuffx += "_";
		filesuffx += pItr1->second->getName();
		file += filesuffx;
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
CommonDataMgr::storePicMetaData(PicMetaDataObj *pPicMetaObj)
{
	int appId = pPicMetaObj->getAppId();
	long shareId = pPicMetaObj->getShrId();
	std::string name = pPicMetaObj->getName();
	const std::vector<std::string>& shareIds = pPicMetaObj->getFrndLst();
	for (const std::string& shareId : shareIds)
	{
  		CommonElem& elem = commonElems[appId][std::stol(shareId)];	
		struct timeval tv;
		gettimeofday(&tv, NULL);
		std::ostringstream valstream;
		valstream  << tv.tv_sec << ";";	
		LckFreeLstSS &lstSS = elem.picShareInfo[shareId];
		lstSS.insertOrUpdate(name, valstream.str());
	}
	std::unique_ptr<PicMetaDataObj> pPicMetaUPtr(pPicMetaObj);
	fdPicMetaMp[pPicMetaObj->getFd()] = std::move(pPicMetaUPtr);
	return;
}

void
CommonDataMgr::storeLstShareInfo(int appId, long shareId, const std::vector<std::string>& shareIds, const std::string& name)
{
	for (const std::string& shareId : shareIds)
	{
		long shId = std::stol(shareId);
  		CommonElem& elem = commonElems[appId][shId];
		struct timeval tv;
		gettimeofday(&tv, NULL);
		std::ostringstream valstream;
		valstream  << tv.tv_sec << ";";	
		LckFreeLstSS &lstSS = elem.lstShareInfo[shareId];
		lstSS.insertOrUpdate(name, valstream.str());
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
		elem.deviceTokens.getVals(tokens);		
	}
	return;
}

std::string
CommonDataMgr::updateLstShareInfo(int appId, long shareId, const std::string& devId, const std::string& name)
{
		
	CommonElem& elem = commonElems[appId][shareId];
	std::string val;
	elem.lstShareInfo.getVal(name, val);
	val += devId;
	val += ";";
	elem.lstShareInfo.insertOrUpdate(name, val);
	return val;
}

void
CommonDataMgr::getShareLists(int appId, long shareId, const std::string& devId, std::map<std::string, std::string>& lstNameMp)
{
	CommonElem& elem = commonElems[appId][shareId];
	std::map<std::string, std::string> names;
	elem.lstShareInfo.getKeyVals(names);
	for (auto pItr = names.begin(); pItr != names.end(); ++pItr)
	{
		if (pItr->second.find(devId) != std::string::npos)
			continue;
		std::string list;
		if (elem.items.getVal(pItr->first, list))
			lstNameMp[pItr->first] = list;
	}
	return;
}
