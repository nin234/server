#include <PicDistribDAO.h>


PicDistribDAO::PicDistribDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/picDistribRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open picDistribRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}

}

PicDistribDAO::~PicDistribDAO()
{
    delete m_db;
}
