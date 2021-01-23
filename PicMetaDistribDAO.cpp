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
