#include <CommonDataMgr.h>
#include <ArchiveMgr.h>
#include <string>
#include <sstream>
#include <sys/time.h>

CommonDataMgr::CommonDataMgr()
{
	std::shared_ptr<CommonArchvr> pCommonArch = ArchiveMgr::Instance().getCommonArchvr();
	if (pCommonArch)
	{
		pCommonArch->populateArchvItems([&](long shareId, const std::string& name, const std::string& templList){storeArchiveItem(shareId, name, templList);});
		pCommonArch->populateItem([&](long shareId, const std::string& name, const std::string& list){storeItem(shareId, name, list);});
		pCommonArch->populateShareLst([&](long shareId, const std::string& name, const std::string& list){storeLstShareInfo(shareId, name, list);});
		pCommonArch->populateDeviceTkn([&](long shareId, const std::string& devId, const std::string& devTkn){storeDeviceTkn(shareId, devId, devTkn);});
	}
}

CommonDataMgr::~CommonDataMgr()
{

}

void
CommonDataMgr::storeArchiveItem(long shareId, const std::string& name, const std::string& templList)
{
  CommonElem& elem = commonElems[shareId];
  elem.archvItems.insert(name, templList);
  return;                                
}                                              

void
CommonDataMgr::storeItem(long shareId, const std::string& name, const std::string& list)
{
  	CommonElem& elem = commonElems[shareId];
  	elem.items.insert(name, list);
	return;
}

void
CommonDataMgr::storeLstShareInfo(long shareId, const std::string& name, const std::string& list)
{
  	CommonElem& elem = commonElems[shareId];
  	elem.lstShareInfo.insert(name, list);
	return;
}


void
CommonDataMgr::storeDeviceTkn(long shareId, const std::string& devId, const std::string& devTkn)
{
  	CommonElem& elem = commonElems[shareId];
  	elem.deviceTokens.insert(devId, devTkn);
	return;
}

void
CommonDataMgr::storeLstShareInfo(const std::vector<std::string>& shareIds, const std::string& name)
{
	for (const std::string& shareId : shareIds)
	{
  		CommonElem& elem = commonElems[std::stol(shareId)];	
		struct timeval tv;
		gettimeofday(&tv, NULL);
		std::ostringstream valstream;
		valstream  << tv.tv_sec << ";";	
		elem.lstShareInfo.insertOrUpdate(name, valstream.str());
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
CommonDataMgr::getDeviceTkns(const std::vector<std::string>& shareIds, std::vector<std::string>& tokens)
{
	for (const std::string& shareId : shareIds)
	{
  		CommonElem& elem = commonElems[std::stol(shareId)];
		elem.deviceTokens.getVals(tokens);		
	}
	return;
}

std::string
CommonDataMgr::updateLstShareInfo(long shareId, const std::string& devId, const std::string& name)
{
		
	CommonElem& elem = commonElems[shareId];
	std::string val;
	elem.lstShareInfo.getVal(name, val);
	val += devId;
	val += ";";
	elem.lstShareInfo.insertOrUpdate(name, val);
	return val;
}

void
CommonDataMgr::getShareLists(long shareId, const std::string& devId, std::map<std::string, std::string>& lstNameMp)
{
	CommonElem& elem = commonElems[shareId];
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
