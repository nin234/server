#include <PicMetaDistribDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>

PicMetaDistribDAO::PicMetaDistribDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/picMetaDistribRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open picMetaDistribRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}


}

PicMetaDistribDAO::~PicMetaDistribDAO()
{
    delete m_db;
}

bool
PicMetaDistribDAO::del(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
    std::stringstream key;
    key << pPicMetaObj->getAppId() << "|" << pPicMetaObj->getShrId() << "|" << pPicMetaObj->getName();
    rocksdb::Status status;
    status = m_db->Delete(rocksdb::WriteOptions(), key.str());
    if (!status.ok())
    {
       std::cout << "Failed to delete from picMetaDistribRocks key=" << key.str() << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        return false; 
    }
       std::cout << "Deleted from picMetaDistribRocks key=" << key.str() << " " << __FILE__ << ":" << __LINE__ << std::endl; 
    return true;
}

std::list<std::shared_ptr<PicMetaDataObj>>
PicMetaDistribDAO::getAll()
{
    std::list<std::shared_ptr<PicMetaDataObj>> picMetaDatas;
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    for (pItr->SeekToFirst(); pItr->Valid(); pItr->Next()) 
    {
        std::string keyStr = pItr->key().ToString();

    }

    return picMetaDatas;
}

bool
PicMetaDistribDAO::store(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
    std::stringstream key;
    key << pPicMetaObj->getAppId() << "|" << pPicMetaObj->getShrId() << "|" << pPicMetaObj->getName();
    std::stringstream value;
    value << pPicMetaObj->getFrnLstStr() << "|" << pPicMetaObj->getPicLen();
    rocksdb::Status status;
    status  = m_db->Put(rocksdb::WriteOptions(), key.str(), value.str());
    if (status.ok())
    {
        std::cout << "Stored distribution Pic Meta data=" << *pPicMetaObj << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    }
    else
    {
        std::cout << "Failed to Store distribution Pic Meta data=" << *pPicMetaObj << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    return true;
}
