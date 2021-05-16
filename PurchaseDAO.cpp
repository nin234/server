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
    key << shareId << "|" << appId << "|" << purchaseId;
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

bool 
PurchaseDAO::get(int appId, long shareId, const std::string& devId, std::string& purchaseId)
{

    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    std::stringstream key;
    key << shareId;
    std::string prefix = key.str();
    for (pItr->Seek(prefix); pItr->Valid();
       pItr->Next()) 
    {

        std::string keyStr = pItr->key().ToString();
        if (keyStr.rfind(prefix, 0) == 0)
        {
           auto keyVec = Util::split(keyStr, '|');
           if (keyVec.size() != 3)
           {
                std::cout << Util::now() << "Invalid size for keyVec=" << keyVec.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;    
                continue;
           }
           
           purchaseId = keyVec[2];
           std::cout << "Found purchaseId=" << purchaseId << " for appId=" << appId << " shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;   
           return true;
        }
    }

    std::cout << "Failed to find purchaseId for appId=" << appId << " shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;   
    return false;
}

