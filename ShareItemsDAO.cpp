#include <ShareItemsDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>
#include <memory>

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

void 
ShareItemsDAO::getShareLists(long appId, long shareId, std::map<shrIdLstName, std::string>& lstNameMp)
{
    std::cout << Util::now() << "Getting share list from rocksdb"<< " " << __FILE__ << ":" << __LINE__ << std::endl;    
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    std::stringstream key;
    key << appId << "|" << shareId;
    
    std::string prefix = key.str();
    for (pItr->Seek(prefix); pItr->Valid();
       pItr->Next()) 
    {
        std::string keyStr = pItr->key().ToString();
        if (keyStr.rfind(prefix, 0) == 0)
        {
           shrIdLstName shlst; 
           auto keyVec = Util::split(keyStr, '|');
           if (keyVec.size() != 4)
           {
                std::cout << Util::now() << "Invalid size for keyVec=" << keyVec.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;    
                continue;
           }
           shlst.shareId = std::stol(keyVec[2]);
           shlst.lstName = keyVec[3];
            lstNameMp[shlst] = pItr->value().ToString(); 
        } 
        else
        {
            break;
        }
    }
}



void 
ShareItemsDAO::delShareLists(long appId, long shareId)
{
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    std::stringstream key;
    key << appId << "|" << shareId;
    
    std::string prefix = key.str();
    for (pItr->Seek(prefix); pItr->Valid();
       pItr->Next()) 
    {
        std::string keyStr = pItr->key().ToString();
        if (keyStr.rfind(prefix, 0) == 0)
        {
            std::cout << Util::now() << "Deleted item key=" << keyStr << " list="
                       << pItr->value().ToString();
             m_db->Delete(rocksdb::WriteOptions(), pItr->key());
        } 
        else
        {
            break;
        }
    }
}
