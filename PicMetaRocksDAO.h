#ifndef _PICMETAROCKSDAO_H_
#define _PICMETAROCKSDAO_H_

#include <rocksdb/db.h>

class PicMetaRocksDAO
{
    rocksdb::DB *m_db;

    public:

        PicMetaRocksDAO();

        virtual ~PicMetaRocksDAO();
};

#endif
