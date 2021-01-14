#ifndef _PICDISTRIBDAO_H_
#define _PICDISTRIBDAO_H_

#include <rocksdb/db.h>

class PicDistribDAO
{
    rocksdb::DB *m_db;

  public:

    PicDistribDAO();
    virtual ~PicDistribDAO();
};

#endif
