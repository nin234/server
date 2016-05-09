#ifndef _COMMON_DATA_MGR_H_ 
#define _COMMON_DATA_MGR_H_
 
#include <HashMap.h>
#include <map>
#include <string>
#include <mutex>
#include <vector>
#include <memory>
#include <MessageObjs.h>
#include <unordered_map>
#include <StorageElems.h>
  
  
    
  class CommonDataMgr
  {               
                  HashMap<long, CommonElem> commonElems[NO_OF_APPS];
		  std::unordered_map<int, std::unique_ptr<PicMetaDataObj>> fdPicMetaMp;
		  std::unordered_map<int, int> fdFdMp;

          
          public: 
                  CommonDataMgr();
                  virtual ~CommonDataMgr();
                  void storeArchiveItem(int appId, long shareId, const std::string& name, const std::string& templList);
                  void storeItem(int appId, long shareId, const std::string& name, const std::string& list);
                  void storeLstShareInfo(int appId, long shareId, const std::string& name, const std::string& list);
                  void storeDeviceTkn(int appId, long shareId, const std::string& devId, const std::string& devTkn);
		  void storeLstShareInfo(int appId, long shareId, const std::vector<std::string>& shareIds, const std::string& name);
                  void storePicMetaData(PicMetaDataObj *pPicMetaObj);
		  bool storePic(PicObj *pObj);
		  static CommonDataMgr& Instance();
		  void getDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
		  void getShareLists(int appId, long shareId, const std::string& devId, std::map<std::string, std::string>& lstNameMp);
		  std::string updateLstShareInfo(int appId, long shareId, const std::string& devId, const std::string& name);
  };
  #endif

