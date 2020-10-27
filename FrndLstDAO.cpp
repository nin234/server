#include <FrndLstDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>

FrndLstDAO::FrndLstDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/frndLstRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open shareIdRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}
}

FrndLstDAO::~FrndLstDAO()
{
    delete m_db;
}


