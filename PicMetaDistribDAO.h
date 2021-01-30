#ifndef PICMETADISTRIBDAO_H_
#define PICMETADISTRIBDAO_H_

#include <MessageObjs.h>
#include <memory>
#include <rocksdb/db.h>
#include <list>

class PicMetaDistribDAO
{
    rocksdb::DB *m_db;

  public:

    PicMetaDistribDAO();

    virtual ~PicMetaDistribDAO();

    bool store(std::shared_ptr<PicMetaDataObj> pPicMetaObj);

    bool del(std::shared_ptr<PicMetaDataObj> pPicMetaObj);

    std::list<std::shared_ptr<PicMetaDataObj>> getAll();
};

#endif
