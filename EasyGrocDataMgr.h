#ifndef _EASYGROC_DATA_MGR_H_ 
#define _EASYGROC_DATA_MGR_H_
 
#include <HashMap.h>
#include <map>
#include <string>
#include <mutex>
#include <vector>

  
  
    
  class EasyGrocDataMgr
  {               
                  HashMap<long, EasyGrocElem> grocListElems;
          
          public: 
                  EasyGrocDataMgr();
                  virtual ~EasyGrocDataMgr();
                  void storeTemplList(long shareId, const std::string& name, const std::string& templList);
                  void storeList(long shareId, const std::string& name, const std::string& list);
                  void storeLstShareInfo(long shareId, const std::string& name, const std::string& list);
                  void storeDeviceTkn(long shareId, const std::string& devId, const std::string& devTkn);
		  void storeLstShareInfo(const std::vector<std::string>& shareIds, const std::string& name);
		  static EasyGrocDataMgr& Instance();
		  void getDeviceTkns(const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
		  void getShareLists(long shareId, const std::string& devId, std::map<std::string, std::string>& lstNameMp);
		  std::string updateLstShareInfo(long shareId, const std::string& devId, const std::string& name);
  };
  #endif

