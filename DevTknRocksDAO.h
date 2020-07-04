#ifndef _DEVTKNROCKSDAO_H_
#define _DEVTKNROCKSDAO_H_

#include <rocksdb/db.h>

class DevTknRocksDAO
{
	rocksdb::DB *m_db;
	public:

	DevTknRocksDAO();
	virtual ~DevTknRocksDAO();
	 void getDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
      	void getAndroidDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens);
      	void storeDeviceTkn(int appId, long shareId, const std::string& devTkn, const std::string& platform);
};
#endif
