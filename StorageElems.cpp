#include <StorageElems.h>


void 
CommonElem::lstShareInsert(long shareId, const std::string& name, const std::string& val)
{
	std::lock_guard<std::mutex> lock(lstShareMtx);
        LckFreeLstSS &lstSS = lstShareInfo[shareId];
        lstSS.insertOrUpdate(name, val);
	return;
}

void 
CommonElem::lstShareDel(long shareId, const std::string& name)
{
	std::lock_guard<std::mutex> lock(lstShareMtx);
	auto pItr = lstShareInfo.find(shareId);
	if (pItr != lstShareInfo.end())
	{
		pItr->second.erase(name);
		if (pItr->second.isEmpty())
			lstShareInfo.erase(pItr);
	}
	return;
}

void
CommonElem::getShareLists(std::map<long, std::vector<std::string>> & shIdItemNames)
{
	std::lock_guard<std::mutex> lock(lstShareMtx);
	for(auto pItr = lstShareInfo.begin(); pItr != lstShareInfo.end(); ++pItr)
	{
		std::vector<std::string> itemNames;
		pItr->second.getKeys(itemNames);
		shIdItemNames[pItr->first] = itemNames;
	}
	return;
}


void 
CommonElem::templLstShareInsert(long shareId, const std::string& name, const std::string& val)
{
	std::lock_guard<std::mutex> lock(lstShareMtx);
        LckFreeLstSS &lstSS = templLstShareInfo[shareId];
        lstSS.insertOrUpdate(name, val);
	return;
}

void 
CommonElem::templLstShareDel(long shareId, const std::string& name)
{
	std::lock_guard<std::mutex> lock(lstShareMtx);
	auto pItr = templLstShareInfo.find(shareId);
	if (pItr != templLstShareInfo.end())
	{
		pItr->second.erase(name);
		if (pItr->second.isEmpty())
			templLstShareInfo.erase(pItr);
	}
	return;
}

void
CommonElem::getTemplShareLists(std::map<long, std::vector<std::string>> & shIdItemNames)
{
	std::lock_guard<std::mutex> lock(lstShareMtx);
	for(auto pItr = templLstShareInfo.begin(); pItr != templLstShareInfo.end(); ++pItr)
	{
		std::vector<std::string> itemNames;
		pItr->second.getKeys(itemNames);
		shIdItemNames[pItr->first] = itemNames;
	}
	return;
}

void 
CommonElem::picShareInsert(long shareId, const std::string& name, long val)
{
	std::lock_guard<std::mutex> lock(picShareMtx);
        LckFreeLstSL &lstSL = picShareInfo[shareId];
        lstSL.insertOrUpdate(name, val);
	return;
}

void 
CommonElem::picShareDel(long shareId, const std::string& name)
{
	std::lock_guard<std::mutex> lock(picShareMtx);
	auto pItr = picShareInfo.find(shareId);
	if (pItr != picShareInfo.end())
	{
		pItr->second.erase(name);
		if (pItr->second.isEmpty())
			picShareInfo.erase(pItr);
	}
	return;
}

void
CommonElem::getSharePics(std::map<long, std::map<std::string, long>> & shIdItemNames)
{
	std::lock_guard<std::mutex> lock(picShareMtx);
	for(auto pItr = picShareInfo.begin(); pItr != picShareInfo.end(); ++pItr)
	{
		std::map<std::string, long> picNamesLens;
		pItr->second.getKeyVals(picNamesLens);
		shIdItemNames[pItr->first] = picNamesLens;
	}
	return;
}


