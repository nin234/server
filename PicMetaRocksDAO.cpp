#include <PicMetaRocksDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>
#include <memory>
#include <map>

PicMetaRocksDAO::PicMetaRocksDAO()
{
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "/home/ninan/data/picMetaRocks", &m_db);

    if (!status.ok())
    {
        std::cout << "Failed to open picMetaRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        exit(0);
    }
}

PicMetaRocksDAO::~PicMetaRocksDAO()
{
    delete m_db;
}

void
PicMetaRocksDAO::store(int appId, 
                 long shareIdSender, 
                 const std::string& picName,
                 long picLen,
                 const std::vector<std::string>& shareIdsRecvs)
{

    for (const auto& recvShareId : shareIdsRecvs)
    {
        std::stringstream key;
        key << appId << "|" << recvShareId << "|" <<  shareIdSender << "|"
            << picName;
        std::stringstream value;
        value << picLen << "|" << false;
        rocksdb::Status status;
        status = m_db->Put(rocksdb::WriteOptions(), key.str(), value.str());
        if (status.ok())
        {
            std::cout << Util::now() << "Storing picture metadata appId=" <<appId << " sender=" << shareIdSender << " receiver=" << recvShareId <<
            " name=" << picName << " picLen=" << picLen << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        }
        else
        {
            std::cout << Util::now() << "Failed to store picture metadata appId=" <<appId << " sender=" << shareIdSender << " receiver=" << recvShareId <<
            " name=" << picName << " picLen=" << picLen << " " << __FILE__ << ":" << __LINE__ << std::endl; 

        }
    }
}

void
PicMetaRocksDAO::updateStatus(int appId, 
                 long shareIdSender, 
                 const std::string& picName,
                 long shareIdRecv)
{
    std::stringstream key;
    key << appId << "|" << shareIdRecv << "|" << shareIdSender << "|"
        << picName;
    std::string value;
    rocksdb::Status status;
    status = m_db->Get(rocksdb::ReadOptions(), key.str(), &value);
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to update PicMeta status key=" << key.str() << " " << __FILE__ << ":" << __LINE__ << std::endl; 
    }
    else
    {
        auto valVec = Util::split(value, '|');
        std::stringstream valstr;
        valstr << valVec[0] << "|" << false;
        status = m_db->Put(rocksdb::WriteOptions(), key.str(), valstr.str());
        if (status.ok())
        {
            std::cout << Util::now() << "Updated PicMeta status key=" << key.str() << " value=" << valstr.str() << " " << __FILE__ << ":" << __LINE__ << std::endl; 

        }
        else
        {
            std::cout << Util::now() << "Failed to Update status key=" << key.str() << " value=" << valstr.str() << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        }
    }
}

void
PicMetaRocksDAO::del(int appId, 
                 long shareIdSender, 
                 const std::string& picName,
                 long shareIdRecv)
{

    std::stringstream key;
    key << appId << "|" << shareIdRecv << "|" << shareIdSender << "|"
        << picName;
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    std::string prefix = key.str();
    std::vector<std::string> keys; 
    for (pItr->Seek(prefix); pItr->Valid();
       pItr->Next()) 
    {
        std::string keyStr = pItr->key().ToString();
        keys.push_back(keyStr);
    }
    
    for (const auto& k : keys)
    {
        rocksdb::Status status;
        status = m_db->Delete(rocksdb::WriteOptions(), k);
        if (!status.ok())
        {
            std::cout << "Failed to Deleted pic meta for key=" << k << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        }
        else
        {
            std::cout << "Deleted pic meta for key=" << k << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        }
    }
}

void
PicMetaRocksDAO::get(int appId, long shareId, std::vector<shrIdLstName>& picNamesShId)
{
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    std::stringstream key;
    key << appId << "|" << shareId;

    std::map<std::string, std::string> updatedItems;
    std::string prefix = key.str();
    std::cout << Util::now() << "Getting PicMetaData from rocksdb key_prefix=" << prefix<< " " << __FILE__ << ":" << __LINE__ << std::endl;  
    for (pItr->Seek(prefix); pItr->Valid();
       pItr->Next()) 
    {
        std::string keyStr = pItr->key().ToString();
        if (keyStr.rfind(prefix, 0) == 0)
        {
           auto keyVec = Util::split(keyStr, '|');
            shrIdLstName shlst;
            shlst.shareId = std::stol(keyVec[2]);
            shlst.appId = appId;
            shlst.lstName = keyVec[3];
            shlst.shareIdElem = shareId;
            auto valVec = Util::split(pItr->value().ToString(), '|');
            if (valVec[1] == "0")
            {
                shlst.picLen =  std::stol(valVec[0]); 
                std::cout << Util::now() << "Getting PicMetaData from rocksdb" <<shlst << " " << __FILE__ << ":" << __LINE__ << std::endl;  
                picNamesShId.push_back(shlst);
                std::stringstream value;
                value << shlst.picLen << "|" << true;
                updatedItems[keyStr] = value.str();
            }
        } 
        else
        {
            break;
        }
    }
    for (const auto& [updatedKey, updatedVal] : updatedItems)
    {
        rocksdb::Status status;
        status = m_db->Put(rocksdb::WriteOptions(), updatedKey, updatedVal);
        if (status.ok())
        {
            std::cout << Util::now() << "Updating picture metadata key=" <<updatedKey << " value=" << updatedVal << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        }
        else
        {
            std::cout << Util::now() << "Failed to Update picture metadata key=" <<updatedKey << " value=" << updatedVal << " " << __FILE__ << ":" << __LINE__ << std::endl; 

        }
    }
}
