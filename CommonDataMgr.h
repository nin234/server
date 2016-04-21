#ifndef _COMMON_DATA_MGR_H_ 
#define _COMMON_DATA_MGR_H_
 
#include <HashMap.h>
#include <map>
#include <string>
#include <mutex>
#include <vector>

  
  
    
  class CommonDataMgr
  {               
                  HashMap<long, CommonElem> commonElems;
          
          public: 
                  CommonDataMgr();
                  virtual ~CommonDataMgr();
                  void storeArchiveItem(long shareId, const std::string& name, const std::string& templList);
                  void storeItem(long shareId, const std::string& name, const std::string& list);
                  void storeLstShareInfo(long shareId, const std::string& name, const std::string& list);
                  void storeDeviceTkn(long shareId, const std::string& devId, const std::string& devTkn);
		  void storeLstShareInfo(const std::vector<std::string>& shareIds, const std::string& name);
		  static CommonDataMgr& Instance();
		  void getDeviceTkns(const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
		  void getShareLists(long shareId, const std::string& devId, std::map<std::string, std::string>& lstNameMp);
		  std::string updateLstShareInfo(long shareId, const std::string& devId, const std::string& name);
  };
  #endif

