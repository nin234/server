#include <ShareItemsDistribDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>
#include <memory>

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
ShareItemsDistribDAO::del(std::shared_ptr<LstObj> pLstObj)
{

    std::stringstream key;
    key << pLstObj->getAppId() << "|" << pLstObj->getShrId() << "|"
        << pLstObj->getName(); 
    rocksdb::Status status;
    status = m_db->Delete(rocksdb::WriteOptions(), key.str());
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to delete distrib Item=" << *pLstObj  << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    std::cout << Util::now() << "Deleted distrib Item=" << *pLstObj  << " " << __FILE__ << ":" << __LINE__ << std::endl;    

    return true;
}

std::list<std::shared_ptr<LstObj>>
ShareItemsDistribDAO::getAll()
{
    std::list<std::shared_ptr<LstObj>> shareItems;
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    for (pItr->SeekToFirst(); pItr->Valid(); pItr->Next()) 
    {
        std::string keyStr = pItr->key().ToString();
        auto keyVec = Util::split(keyStr, '|');
        if (keyVec.size() != 3)
        {
            std::cout << Util::now() << "Invalid size for keyVec=" << keyVec.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;    
            continue;
        }
        auto pLstObj = std::make_shared<LstObj>();
        pLstObj->setAppId(std::stoi(keyVec[0]));  
        pLstObj->setShrId(std::stol(keyVec[1]));  
        pLstObj->setName(keyVec[2]);
        pLstObj->setList(pItr->value().ToString());
        shareItems.push_back(pLstObj); 
    }

    return shareItems;
}

bool
ShareItemsDistribDAO::store(std::shared_ptr<LstObj> pLstObj)
{
    std::stringstream key;
    key << pLstObj->getAppId() << "|" << pLstObj->getShrId() << "|"
        << pLstObj->getName(); 
    rocksdb::Status status;
    status  = m_db->Put(rocksdb::WriteOptions(), key.str(), pLstObj->getList());
    if (status.ok())
    {
        std::cout << Util::now() << "Archived distrib Item=" << *pLstObj  << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    }
    else
    {
        std::cout << Util::now() << "Failed to archive distrib Item=" << *pLstObj << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }

    return true;
}
