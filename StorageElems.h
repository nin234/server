#ifndef _STORAGE_ELEMS_H_
#define _STORAGE_ELEMS_H_

#include <string>
#include <map>
#include <HashMapStr.h>
#include <HashMap.h>
#include <iostream>
#include <mutex>
#include <vector>

//create a lock free list/map of template lists and ordinary items

  struct CommonElem
  {       
          HashMapStr archvItems;
          HashMapStr items;
        HashMapStr templItems;
          std::string deviceToken;
      std::string os;
	void lstShareInsert(long shareId, const std::string& name, const std::string& val);
	void lstShareDel(long shareId, const std::string& name);
	void getShareLists(std::map<long, std::vector<std::string>>& shIdItemNames);
	void templLstShareInsert(long shareId, const std::string& name, const std::string& val);
	void templLstShareDel(long shareId, const std::string& name);
	void getTemplShareLists(std::map<long, std::vector<std::string>>& shIdItemNames);
	void picShareInsert(long shareId, const std::string& name,  long val);
	void picShareDel(long shareId, const std::string& name);
	void getSharePics(std::map<long, std::map<std::string, long>>& shIdItemNames);
	private:
	  std::map<long, LckFreeLstSS> lstShareInfo;
      	  std::map<long, LckFreeLstSS> templLstShareInfo;
	  std::map<long, LckFreeLstSL> picShareInfo;
  };

 struct shrIdLstName
{
	long shareId;
	std::string lstName;
	long picLen;
	int fd;
	int appId;
	long shareIdElem;
	int picSoFar;
	bool operator < (const shrIdLstName& shlst) const
	{
		if (shareId != shlst.shareId)
			return shareId < shlst.shareId;
		else
			return lstName < shlst.lstName;

	}

	shrIdLstName()
	{
		picSoFar = 0;
	}
};
//Each share_id will have an instance of CommonElem
//lstShareInfo is a linked list and each element in the list is a key value
//pair of shareId and list Name issue a shareId can more than one item to share so the key should be shareId:::listName value is listName
//Each item should have an expiry time



#endif
