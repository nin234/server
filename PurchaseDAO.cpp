#include <PurchaseDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>


PurchaseDAO::PurchaseDAO()
{
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/purchaseRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open purchaseRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}	
}

PurchaseDAO::~PurchaseDAO()
{
	delete m_db;
}

void 
PurchaseDAO::store(int appId, long shareId, const std::string& devId, const std::string& purchaseId)
{
    std::stringstream key;
    key << appId << "|" << shareId << "|" << purchaseId;
    rocksdb::Status status;
    status  = m_db->Put(rocksdb::WriteOptions(), key.str(), devId);
    
    if (!status.ok())
    {
        std::cout  << Util::now()<< "Failed to store purchase info appId=" << appId << " shareId=" << shareId << " purchaseId=" << purchaseId
        << " deviceId=" << devId << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    }
    std::cout << Util::now() << "Stored purchase info appId=" << appId << " shareId=" << shareId << " purchaseId=" << purchaseId
        << " deviceId=" << devId << " " << __FILE__ << ":" << __LINE__ << std::endl;    

}


