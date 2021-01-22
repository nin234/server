#include <PicMetaDistribDAO.h>

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

    return true;
}
