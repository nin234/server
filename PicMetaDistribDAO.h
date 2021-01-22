#ifndef PICMETADISTRIBDAO_H_
#define PICMETADISTRIBDAO_H_

#include <MessageObjs.h>
#include <memory>
#include <rocksdb/db.h>

class PicMetaDistribDAO
{
    rocksdb::DB *m_db;

  public:

    PicMetaDistribDAO();

    virtual ~PicMetaDistribDAO();

    bool store(std::shared_ptr<PicMetaDataObj> pPicMetaObj);
};

#endif
