#include <DevTknRocksDAO.h>
#include <iostream>
#include <sstream>


DevTknRocksDAO::DevTknRocksDAO()
{
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/devTknRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open devTknRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}	
}

DevTknRocksDAO::~DevTknRocksDAO()
{
	delete m_db;
}

void
DevTknRocksDAO::getDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens)
{

}

void
DevTknRocksDAO::getAndroidDeviceTkns(int appId, const std::vector<std::string>& shareIds, std::vector<std::string>& tokens)
{

}

void
DevTknRocksDAO::storeDeviceTkn(int appId, long shareId, const std::string& devTkn, const std::string& platform)
{
    std::stringstream key;
    key << appId << "|" << shareId << "|" << platform;
    rocksdb::Status status = m_db->Put(rocksdb::WriteOptions(), key.str(), devTkn);
    
    if (!status.ok())
    {
        std::cout << "Failed to store device token appId=" << appId << " shareId=" << shareId << " token=" << devTkn << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    }
    std::cout << "Stored device token appId=" << appId << " shareId=" << shareId << " token=" << devTkn << " " << __FILE__ << ":" << __LINE__ << std::endl;    
}
