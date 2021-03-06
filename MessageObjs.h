#ifndef _MESSAGE_OBJS_H_
#define _MESSAGE_OBJS_H_

#include <string>
#include <vector>
#include <Constants.h>
#include <iostream>


class MsgObj
{
	int msgTyp;
	int fd;
	int appId;
	public:
	virtual ~MsgObj(){}
	 int getMsgTyp()const {return msgTyp;}
	void setMsgTyp(int mtyp) {msgTyp = mtyp;}
	int getFd() const {return fd;}
	void setFd(int fdes) {fd = fdes;}
	int getAppId() const {return appId;}
	void setAppId (int appid) {appId = appid;}

};

class MsgObjDeltr
{
	public:
		void operator ()(MsgObj *ptr);
};


class GetRemoteHostObj : public MsgObj
{
        long shareId;

    public:

        virtual ~GetRemoteHostObj(){}
        long getShareId() const {return shareId;}
        void setShareId(long shId){shareId = shId;}
};

std::ostream& operator << (std::ostream& os, const GetRemoteHostObj& getRemoteHost);

class StorePurchasedObj : public MsgObj
{
        long shareId;
        std::string purchaseId;
        std::string deviceId;

    public:

        virtual ~StorePurchasedObj(){}
        long getShareId() const {return shareId;}
        void setShareId(long shId){shareId = shId;}
		const std::string& getPurchaseId() const {return purchaseId;}
		void setPurchaseId(const char *pPurId) {purchaseId = pPurId;}
		const std::string& getDeviceId() const {return deviceId;}
		void setDeviceId(const char *pDevId) {deviceId = pDevId;}
};

std::ostream& operator << (std::ostream& os, const StorePurchasedObj& storePurchase);


class PicObj : public MsgObj
{
		char buf[MAX_BUF];
		int len;
		int buflen;
	public:
		PicObj();
		virtual ~PicObj(){}
		int getLen() {return len;}
		void setLen(int ln) {len = ln;}
		int getBufLen() {return buflen;}
		void setBuf(char *pb, int pblen);
		const char *getBuf() {return buf;}
};

class PicMetaDataObj;

std::ostream& operator << (std::ostream& os, const PicMetaDataObj& picMetaData);

class PicMetaDataObj : public MsgObj
{
		std::string name;
		std::vector<std::string> frndLst;
		std::string frndLstStr;
		long shrId;
		int picLen;
		int writtenLen;
	public:
		virtual ~PicMetaDataObj(){}
		PicMetaDataObj();
		friend std::ostream& operator << (std::ostream& os, const PicMetaDataObj& picMetaData);
		long  getShrId() const {return shrId;}
		void setShrId(long sid) {shrId = sid;}
		std::string getName() const {return name;}
		void setName(const char *pName, int len);
        void setName(const std::string& nm){name = nm;}
		const std::vector<std::string>& getFrndLst(){return frndLst;}
		std::vector<std::string> getLocalFrndLst() const;
		void setFrndLst(const std::vector<std::string>& fl);
		int getPicLen() const {return picLen;}
		void setPicLen(int len){picLen = len;}
		int getWrittenLen() {return writtenLen;}
		void setWrittenLen(int len) {writtenLen = len;}
		std::string getFrnLstStr() const {return frndLstStr;}
		void setFrndLstStr(const char *pFlist, int len); 
		void setFrndLstStr(const std::string& flist);
        
};

class ShareIdObj : public MsgObj
{
	long trnId;
	long shrId;
    std::string deviceId;
	public:
		virtual ~ShareIdObj(){}
		ShareIdObj():shrId(0){}
		long getTrnId() {return trnId;}
		void setTrnId(long tid) {trnId = tid;}
		long  getShrId() {return shrId;}
		void setShrId(long sid) {shrId = sid;}
		const std::string& getDeviceId() const {return deviceId;}
		void setDeviceId(const char *pDevId) {deviceId = pDevId;}
};

struct shrIdTrnId
{
	long shrId;
	long trnId;
};

class FrndLstObj : public MsgObj
{
	std::string frndLst;
	long shrId;
	public:
		virtual ~FrndLstObj(){}
		FrndLstObj() :shrId(0) {}
		long  getShrId() const {return shrId;}
		void setShrId(long sid) {shrId = sid;}
		const std::string& getFrndLst() const {return frndLst;}
		void setFrndLst(const char *pLst) {frndLst = pLst;}
		
};

class DeviceTknObj : public MsgObj
{
	std::string deviceTkn;
	std::string deviceId;
    std::string platform;
	long shrId;
	public:
		virtual ~DeviceTknObj(){}
		DeviceTknObj() :shrId(0) {}
		long  getShrId() const {return shrId;}
   
		void setShrId(long sid) {shrId = sid;}
		const std::string& getDeviceTkn() const {return deviceTkn;}
		void setDeviceTkn(const char *pLst) {deviceTkn = pLst;}
		const std::string& getDeviceId() const {return deviceId;}
		void setDeviceId(const char *pLst) {deviceId = pLst;}
        const std::string& getPlatform() const {return platform;}
        void setPlatform(const char *pOs) {platform = pOs;}
};


struct shrdIdSize
{
	int size;
	long shrId;
};

class TemplLstObj : public MsgObj
{
		long shrId;
		std::string name;
		std::string templList;
	public:
		virtual ~TemplLstObj(){}
		TemplLstObj() : shrId(0) {}
		long getShrId() const {return shrId;}	
		void setShrId(long sid) {shrId = sid;}
		const std::string& getName() const {return name;}
		void setName(const char *pName, int len);
		const std::string& getTemplList(){return templList;}
		void setTemplList(const char *pList, int len);

};

struct shrdIdTemplSize
{
	long shrId;
	int appId;
	bool del;
	int name_len;
	int list_len;
	shrdIdTemplSize():del(false), name_len(0), list_len(0) {}
};

struct devTknArchv;

std::ostream& operator << (std::ostream& os, const devTknArchv& devTkInfo);
struct devTknArchv
{
	long shareId;
	int appId;
	int tkn_len;
    int platform;
};

struct shareInfo;

std::ostream& operator << (std::ostream& os, const shareInfo& shInfo);

struct shareInfo
{
	int appId;
	long shrIdLst;
	long shrId;
	bool del;
	int name_len;
	int list_len;
    friend std::ostream& operator << (std::ostream& os, const shareInfo& shInfo);
};


struct picShareInfo : public shareInfo
{
	int pic_len;
};

std::ostream& operator << (std::ostream& os, const picShareInfo& shInfo);


class LstObj : public MsgObj
{
		long shrId;
		std::string name;
		std::string list;
	public:
		virtual ~LstObj(){}
		LstObj() : shrId(0) {}
		long getShrId() const {return shrId;}	
		void setShrId(long sid) {shrId = sid;}
		const std::string& getName() const {return name;}
		void setName(const char *pName, int len);
		const std::string& getList() const {return list;}
		void setList(const char *pList, int len);
        void setName(std::string nm) {name = nm;}
        void setList(std::string lst) {list = lst;}
};

std::ostream& operator << (std::ostream& os, const LstObj& lstObj);

class GetItemObj;

std::ostream& operator << (std::ostream& os, const GetItemObj& getItem);

class PicDoneObj : public MsgObj
{
	long shrId;
	std::string picName;
	long picShareId;
	public:
		virtual ~PicDoneObj(){}
		PicDoneObj() : shrId(0){}
		long getShrId() const {return shrId;}	
		void setShrId(long sid) {shrId = sid;}
                const std::string& getPicName() const {return picName;}
		void setPicName(const std::string& pname) {picName = pname;}
		long getPicShareId() const {return picShareId;}	
		void setPicShareId(long sid) {picShareId = sid;}
};

std::ostream& operator << (std::ostream& os, const PicDoneObj& picItem);

class GetItemObj : public PicDoneObj
{
	std::string devId;
	int picRemaining;
    long maxShareId;
	public:
		virtual ~GetItemObj(){}
		GetItemObj():maxShareId(0){}
		const std::string& getDeviceId() const {return devId;}
		void setDeviceId(const std::string& dvId) {devId = dvId;}	
		int getPicRemaining() const {return picRemaining;}
		void setPicRemaining(int picrmng) {picRemaining = picrmng;}		
        long getMaxShareId() const {return maxShareId;}
        void setMaxShareId(long maxId) {maxShareId = maxId;}

};

class ShouldDownLoad : public MsgObj
{

	long shareId;
	std::string name;
	bool download;
	
	public:

		virtual ~ShouldDownLoad(){}
		ShouldDownLoad(){}
		long getShrId() const {return shareId;}
		void setShrId(long sid) {shareId = sid;}
		bool downLoad() const {return download;}
		void setDownLoad(bool dwld) {download = dwld;}
        const std::string& getName() const {return name;}
    void setName(const std::string& nm) {name = nm;}

};

std::ostream& operator << (std::ostream& os, const ShouldDownLoad& shldDwld);
#endif
