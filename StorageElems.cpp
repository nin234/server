#include <StorageElems.h>


void 
CommonElem::lstShareInsert(long shareId, const std::string& name, const std::string& val)
{
        LckFreeLstSS &lstSS = lstShareInfo[shareId];
        lstSS.insertOrUpdate(name, val);
	return;
}

void 
CommonElem::lstShareDel(long shareId, const std::string& name)
{
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
        LckFreeLstSS &lstSS = templLstShareInfo[shareId];
        lstSS.insertOrUpdate(name, val);
	return;
}

void 
CommonElem::templLstShareDel(long shareId, const std::string& name)
{
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
        std::map<std::string, picInfo> &lstSL = picShareInfo[shareId];
	picInfo pic_detail(false, val);
	lstSL[name] = pic_detail;	
	return;
}

void 
CommonElem::picShareDel(long shareId, const std::string& name)
{
	auto pItr = picShareInfo.find(shareId);
	if (pItr != picShareInfo.end())
	{
		for (auto pPicMpItr = pItr->second.begin(); pPicMpItr != pItr->second.end(); ++pPicMpItr)
		{
			if (pPicMpItr->first.find(name) != std::string::npos)
			{
				pItr->second.erase(pPicMpItr);
					break;
			}
		}
		if (pItr->second.empty())
			picShareInfo.erase(pItr);
	}
	return;
}

void
CommonElem::getSharePics(std::map<long, std::map<std::string, long>> & shIdItemNames)
{
	for(auto pItr = picShareInfo.begin(); pItr != picShareInfo.end(); ++pItr)
	{
		std::map<std::string, long> picNamesLens;
		for (auto pPicMpItr = pItr->second.begin(); pPicMpItr != pItr->second.end(); ++pPicMpItr)
		{
			if (!pPicMpItr->second.getDownload())
			{
				picNamesLens[pPicMpItr->first] = pPicMpItr->second.getPicLen();
				pPicMpItr->second.setDownload(true);
			}
		}
		
		shIdItemNames[pItr->first] = picNamesLens;
	}
	return;
}


