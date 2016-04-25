#ifndef _COMMON_DATA_MGR_H_ 
#define _COMMON_DATA_MGR_H_
 
#include <HashMap.h>
#include <map>
#include <string>
#include <mutex>
#include <vector>

  
  
    
  class CommonDataMgr
  {               
                  HashMap<long, CommonElem> commonElems[NO_OF_APPS];
          
          public: 
                  CommonDataMgr();
                  virtual ~CommonDataMgr();
                  void storeArchiveItem(int appId, long shareId, const std::string& name, const std::string& templList);
                  void storeItem(int appId, long shareId, const std::string& name, const std::string& list);
                  void storeLstShareInfo(int appId, long shareId, const std::string& name, const std::string& list);
                  void storeDeviceTkn(int appId, long shareId, const std::string& devId, const std::string& devTkn);
		  void storeLstShareInfo(int appId, const std::vector<std::string>& shareIds, const std::string& name);
                  void storePicMetaData(int appId, long shareId, const std::string& name, const std::vector<std::string>& shareIds);
		  static CommonDataMgr& Instance();
		  void getDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
		  void getShareLists(int appId, long shareId, const std::string& devId, std::map<std::string, std::string>& lstNameMp);
		  std::string updateLstShareInfo(int appId, long shareId, const std::string& devId, const std::string& name);
  };
  #endif

