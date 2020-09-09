#include <PicMetaRocksDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>

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
