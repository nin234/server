#include <ShareItemsDistribDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>

ShareItemsDistribDAO::ShareItemsDistribDAO()
{
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/shareItemsDistribRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open shareItemsDistribRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}
}

ShareItemsDistribDAO::~ShareItemsDistribDAO()
{
	delete m_db;
}

bool
ShareItemsDistribDAO::del(const std::string& host, int port,
                        std::shared_ptr<LstObj> pLstObj)
{

    std::stringstream key;
    key << host << "|" << port << "|" << pLstObj->getShrId() << "|"
        << pLstObj->getName(); 
    rocksdb::Status status;
    status = m_db->Delete(rocksdb::WriteOptions(), key.str());
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to delete distrib Item=" << *pLstObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    std::cout << Util::now() << "Deleted distrib Item=" << *pLstObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;    

    return true;
}

bool
ShareItemsDistribDAO::store(const std::string& host, int port,
                        std::shared_ptr<LstObj> pLstObj)
{
    std::stringstream key;
    key << host << "|" << port << "|" << pLstObj->getShrId() << "|"
        << pLstObj->getName(); 
    rocksdb::Status status;
    status  = m_db->Put(rocksdb::WriteOptions(), key.str(), pLstObj->getList());
    if (status.ok())
    {
        std::cout << Util::now() << "Archived distrib Item=" << *pLstObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    }
    else
    {
        std::cout << Util::now() << "Failed to archive distrib Item=" << *pLstObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }

    return true;
}
