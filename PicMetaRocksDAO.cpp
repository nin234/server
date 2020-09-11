#include <PicMetaRocksDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>
#include <memory>

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
PicMetaRocksDAO::del(int appId, 
                 long shareIdSender, 
                 const std::string& picName,
                 long shareIdRecv)
{

    std::stringstream key;
    key << appId << "|" << shareIdRecv << "|" << shareIdSender << "|"
        << picName;
    m_db->Delete(rocksdb::WriteOptions(), key.str());
}

void
PicMetaRocksDAO::get(int appId, long shareId, std::vector<shrIdLstName>& picNamesShId)
{
    std::cout << Util::now() << "Getting PicMetaData from rocksdb" << " " << __FILE__ << ":" << __LINE__ << std::endl;  
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
           auto keyVec = Util::split(keyStr, '|');
            shrIdLstName shlst;
            shlst.shareId = std::stol(keyVec[2]);
            shlst.appId = appId;
            shlst.lstName = keyVec[3];
            shlst.shareIdElem = shareId;
            auto valVec = Util::split(pItr->value().ToString(), '|');
            shlst.picLen =  std::stol(valVec[0]); 
            picNamesShId.push_back(shlst);
        } 
        else
        {
            break;
        }
    }
}
