#ifndef _SHAREIDDAO_H_
#define _SHAREIDDAO_H_

#include <rocksdb/db.h>

class ShareIdDAO
{
    rocksdb::DB *m_db;

    public:

    ShareIdDAO();
    
    virtual ~ShareIdDAO();
};
#endif
