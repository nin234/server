#ifndef _MESSAGE_OBJS_H_
#define _MESSAGE_OBJS_H_

#include <string>
#include <vector>
#include <Constants.h>


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
		PicMetaDataObj(){}
		long  getShrId() {return shrId;}
		void setShrId(long sid) {shrId = sid;}
		std::string getName(){return name;}
		void setName(const char *pName, int len);
		const std::vector<std::string>& getFrndLst(){return frndLst;}
		void setFrndLst(const std::vector<std::string>& fl){frndLst = fl;}
		int getPicLen() {return picLen;}
		void setPicLen(int len){picLen = len;}
		int getWrittenLen() {return writtenLen;}
		void setWrittenLen(int len) {writtenLen = len;}
		std::string getFrnLstStr() {return frndLstStr;}
		void setFrndLstStr(const char *pFlist, int len); 
};

class ShareIdObj : public MsgObj
{
	long trnId;
	long shrId;
	public:
		virtual ~ShareIdObj(){}
		ShareIdObj():shrId(0){}
		long getTrnId() {return trnId;}
		void setTrnId(long tid) {trnId = tid;}
		long  getShrId() {return shrId;}
		void setShrId(long sid) {shrId = sid;}
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
	int name_len;
	int list_len;
};

struct shareInfo
{
	int appId;
	long shrIdLst;
	long shrId;
	bool del;
	int name_len;
	int list_len;
};


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
		const std::string& getList(){return list;}
		void setList(const char *pList, int len);

};

class GetItemObj : public MsgObj
{
	long shrId;
	std::string devId;
	public:
		virtual ~GetItemObj(){}
		GetItemObj() : shrId(0){}
		long getShrId() const {return shrId;}	
		void setShrId(long sid) {shrId = sid;}
		const std::string& getDeviceId() {return devId;}
		void setDeviceId(const std::string& dvId) {devId = dvId;}	
};

#endif
