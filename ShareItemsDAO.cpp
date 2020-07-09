#include <ShareItemsDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>

ShareItemsDAO::ShareItemsDAO()
{
	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/shareItemRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open shareItemRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}	
}

ShareItemsDAO::~ShareItemsDAO()
{
	delete m_db;
}

void 
ShareItemsDAO::storeItem(int appId, long shareIdLst, const std::string& name, const std::string& list, const std::vector<std::string>& shareIds)
{
    for (const auto& shareId : shareIds)
    {
        std::stringstream key;
        key << appId << "|" << shareId << "|" << shareIdLst << "|" << name;
        rocksdb::Status status;
        status  = m_db->Put(rocksdb::WriteOptions(), key.str(), list);
        if (status.ok())
        {
            std::cout << Util::now() << "Storing item to share appId=" << appId << " shareToId=" << shareId << " shareFromId=" << shareIdLst << " name=" << name << " list=" << list << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        }
        else
        {
            std::cout << Util::now() << "Failed to store item to share appId=" << appId << " shareToId=" << shareId << " shareFromId=" << shareIdLst << " name=" << name << " list=" << list << " " << __FILE__ << ":" << __LINE__ << std::endl;    

        }
    }
}


