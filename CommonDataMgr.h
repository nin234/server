#ifndef _COMMON_DATA_MGR_H_ 
#define _COMMON_DATA_MGR_H_
 
#include <HashMap.h>
#include <map>
#include <string>
#include <mutex>
#include <array>
#include <vector>
#include <memory>
#include <MessageObjs.h>
#include <unordered_map>
#include <StorageElems.h>
#include <ArchiveSndr.h>
#include <DevTknRocksDAO.h>
#include <ShareItemsDAO.h>
#include <PicMetaRocksDAO.h>
#include <FrndLstMgr.h>
#include <PurchaseDAO.h>
  
  
    
  class CommonDataMgr
  {               
	std::array<CommonElem, HASH_MAP_SIZE> commonElems[NO_OF_APPS];
    std::array<std::mutex, HASH_MAP_SIZE> commonElemsMtx[NO_OF_APPS];
    DevTknRocksDAO m_rocksDAO;
    ShareItemsDAO  m_shareItemDAO;
    PicMetaRocksDAO m_picMetaDAO;
    FrndLstMgr      m_frndLstMgr;
    PurchaseDAO     m_purchaseDAO;
	
	  static thread_local std::unordered_map<int, std::shared_ptr<PicMetaDataObj>> fdPicMetaMp;
	  static thread_local std::unordered_map<int, int> fdFdMp;

	 bool shouldUpload(std::shared_ptr<PicMetaDataObj> pPicMetaObj, int *picOffset); 
	  int openPicFile(long shareId, int appId, const std::string& picName, std::shared_ptr<PicObj> pPicObj);
	  
	 void storePicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj);
          public: 
                  CommonDataMgr();
                  virtual ~CommonDataMgr();
                  void storeArchiveItem(int appId, long shareId, const std::string& name, const std::string& templList);
                  void storeItem(int appId, long shareId, const std::string& name, const std::string& list);
                  void storeLstShareInfo(int appId, long shareId, const std::string& name, const std::string& list);
                    void storeTemplLstShareInfo(int appId, long shareId, const std::string& name, const std::string& list);
      void storeDeviceTkn(int appId, long shareId, const std::string& devTkn, const std::string& platform);
		  void storeLstShareInfo(int appId, long shareId, const std::string& name, long shareIdLst);
		  void storePicMetaInfo(int appId, long shareId, const std::string& name, long shareIdLst, int pic_len);
      		void storeTemplLstShareInfo(int appId, long shareId, const std::string& name, long shareIdLst);
      		void storeTemplLstShareInfo(int appId, long shareIdLst, const std::vector<std::string>& shareIds, const std::string& name);
      		void storeLstShareInfo(int appId, long shareIdLst, const std::vector<std::string>& shareIds, const std::string& name);

      		void storeItemAndLstShareInfo(int appId, long shareIdLst, const std::string& name, const std::string& list, const std::vector<std::string>& shareIds);

             bool storePicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj, int *picOffset);

            void storeStorePurchase(std::shared_ptr<StorePurchasedObj> pStorePurchaseMsg);
            bool getPurchase(std::shared_ptr<StorePurchasedObj> pStorePurchaseMsg);

		  std::vector<std::string> getPicShareIds(int fd);
		  std::string getPicName(int fd);
		  void eraseFdMp(int fd);
		  bool storePic(std::shared_ptr<PicObj> pObj, bool& cleanUpFd);
		  static CommonDataMgr& Instance();
		  void getDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
      void getAndroidDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
		  void getShareLists(int appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp);
		  std::string getFrndList(int appId, long shareId, bool bDontCheckUpdFlag);
		  void delShareLists(int appId, long shareId);
      void getShareTemplLists(int appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp);
		  void getPictureNames(int appId, long shareId, std::vector<shrIdLstName>& picNames);
		  void updateLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName);
		  void updatePicShareInfo(int appId, long shareId, long frndShareId, const std::string& picName);
		  void updatePicShareStatus(int appId, long shareId, long frndShareId, const std::string& picName);
            void updateTemplLstShareInfo(int appId, long shareId, long frndShareId, const std::string& itemName);
            void storeTemplItem(int appId, long shareId, const std::string& name, const std::string& list);

            void storeFrndLst(std::shared_ptr<FrndLstObj> pFrndObj);
            void updateFrndLstStatus(long shareId, int appId);
            std::shared_ptr<PicMetaDataObj> getPicMetaObj(int fd);
  };
  #endif

