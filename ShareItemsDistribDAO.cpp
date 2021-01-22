#include <ShareItemsDistribDAO.h>

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
