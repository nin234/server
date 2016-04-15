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
		pCommonArch->populateArchvItems([&](long shareId, const std::string& name, const std::string& templList){storeTemplList(shareId, name, templList);});
		pCommonArch->populateItem([&](long shareId, const std::string& name, const std::string& list){storeList(shareId, name, list);});
		pCommonArch->populateShareLst([&](long shareId, const std::string& name, const std::string& list){storeLstShareInfo(shareId, name, list);});
		pCommonArch->populateDeviceTkn([&](long shareId, const std::string& devId, const std::string& devTkn){storeDeviceTkn(shareId, devId, devTkn);});
	}
}

CommonDataMgr::~CommonDataMgr()
{

}

void
CommonDataMgr::storeTemplList(long shareId, const std::string& name, const std::string& templList)
{
  EasyGrocElem& elem = grocListElems[shareId];
  elem.templLists.insert(name, templList);
  return;                                
}                                              

void
CommonDataMgr::storeList(long shareId, const std::string& name, const std::string& list)
{
  	EasyGrocElem& elem = grocListElems[shareId];
  	elem.lists.insert(name, list);
	return;
}

void
CommonDataMgr::storeLstShareInfo(long shareId, const std::string& name, const std::string& list)
{
  	EasyGrocElem& elem = grocListElems[shareId];
  	elem.lstShareInfo.insert(name, list);
	return;
}


void
CommonDataMgr::storeDeviceTkn(long shareId, const std::string& devId, const std::string& devTkn)
{
  	EasyGrocElem& elem = grocListElems[shareId];
  	elem.deviceTokens.insert(devId, devTkn);
	return;
}

void
CommonDataMgr::storeLstShareInfo(const std::vector<std::string>& shareIds, const std::string& name)
{
	for (const std::string& shareId : shareIds)
	{
  		EasyGrocElem& elem = grocListElems[std::stol(shareId)];	
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
  		EasyGrocElem& elem = grocListElems[std::stol(shareId)];
		elem.deviceTokens.getVals(tokens);		
	}
	return;
}

std::string
CommonDataMgr::updateLstShareInfo(long shareId, const std::string& devId, const std::string& name)
{
		
	EasyGrocElem& elem = grocListElems[shareId];
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
	EasyGrocElem& elem = grocListElems[shareId];
	std::map<std::string, std::string> names;
	elem.lstShareInfo.getKeyVals(names);
	for (auto pItr = names.begin(); pItr != names.end(); ++pItr)
	{
		if (pItr->second.find(devId) != std::string::npos)
			continue;
		std::string list;
		if (elem.lists.getVal(pItr->first, list))
			lstNameMp[pItr->first] = list;
	}
	return;
}
