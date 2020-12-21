#include <ShareIdDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>

ShareIdDAO::ShareIdDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/shareIdRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open shareIdRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}
	

}

ShareIdDAO::~ShareIdDAO()
{
    delete m_db;
}

long
ShareIdDAO::getMaxShareId()
{
    std::string shareIdStr;
    rocksdb::Status status = m_db->Get(rocksdb::ReadOptions(), "MaxShareId", &shareIdStr);
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed retrieve MaxShareId" << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        return 0; 
    }
        std::cout << Util::now() << "maxShareId=" << shareIdStr << " " << __FILE__ << ":" << __LINE__ << std::endl; 
    return std::stol(shareIdStr);
}

long
ShareIdDAO::get(const std::string& deviceId)
{

    rocksdb::Status status; std::string shareIdStr;
    status = m_db->Get(rocksdb::ReadOptions(), deviceId, &shareIdStr);
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to retrieve shareId for deviceId=" << deviceId << " " << __FILE__ << ":" << __LINE__ << std::endl;  
        return 0;
    }
    std::cout << Util::now() << "deviceId=" << deviceId << " shareId=" << shareIdStr << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    return std::stol(shareIdStr);
}

bool
ShareIdDAO::set(const std::string& deviceId, long shareId)
{

    std::stringstream value;
    value << shareId;
    rocksdb::Status status;
    status = m_db->Put(rocksdb::WriteOptions(), deviceId, value.str());
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to store deviceId=" << deviceId << " shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    std::cout << Util::now() << "Stored deviceId=" << deviceId << " shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    return true;
}

bool
ShareIdDAO::setMaxShareId(long maxShareId)
{
    std::stringstream value;
    value << maxShareId;
    rocksdb::Status status;
    status = m_db->Put(rocksdb::WriteOptions(), "MaxShareId", value.str());
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to store  maxShareId=" << maxShareId << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    std::cout << Util::now() << "Stored maxShareId=" << maxShareId << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    return true;

}
