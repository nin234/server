#include <DistributedDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>


DistributedDAO::DistributedDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/distributedRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open distributedRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}
	
}

DistributedDAO::~DistributedDAO()
{
    delete m_db;
}

bool
DistributedDAO::setNode(int startShareId, int endShareId, const std::map<std::string, int>& hostPortMap)
{
    std::stringstream key;
    key << startShareId;
    rocksdb::Status status;
    std::stringstream value;
    value << endShareId; 
    for (const auto [host, port] : hostPortMap)
    {
        value << "|" << host << "|" << port;
    }

    status = m_db->Put(rocksdb::WriteOptions(), key.str(), value.str());
    if (status.ok())
    {
        std::cout << Util::now() << "Stored Item to Distributed DB startShareId=" << startShareId << " value=" << value.str() << " " << __FILE__ << ":" << __LINE__ << std::endl;   
    }
    else
    {
        std::cout << Util::now() << "Failed store Item to Distributed DB startShareId=" << startShareId << " value=" << value.str() << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        return false;
    }
    return true;
}
