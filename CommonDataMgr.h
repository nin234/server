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
#include <ArchiveSndr.h>
  
  
    
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
                    void storeTemplLstShareInfo(int appId, long shareId, const std::string& name, const std::string& list);
      void storeDeviceTkn(int appId, long shareId, const std::string& devTkn, const std::string& platform);
		  void storeLstShareInfo(int appId, long shareId, const std::vector<std::string>& shareIds, const std::string& name);
      void storeTemplLstShareInfo(int appId, long shareId, const std::vector<std::string>& shareIds, const std::string& name);
                  void storePicMetaData(PicMetaDataObj *pPicMetaObj);
		  std::vector<std::string> getPicShareIds(int fd);
		  std::string getPicName(int fd);
		  void eraseFdMp(int fd);
		  bool storePic(PicObj *pObj);
		  static CommonDataMgr& Instance();
		  void getDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
		  void getShareLists(int appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp);
      void getShareTemplLists(int appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp);
		  void getPictureNames(int appId, long shareId, std::vector<shrIdLstName>& picNames);
		  void updateLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName);
            void updateTemplLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName);
            void storeTemplItem(int appId, long shareId, const std::string& name, const std::string& list);
  };
  #endif

