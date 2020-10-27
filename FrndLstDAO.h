#ifndef _FRNDLSTDAO_H_
#define _FRNDLSTDAO_H_

#include <rocksdb/db.h>

class FrndLstDAO
{
    rocksdb::DB *m_db;

    public:

    FrndLstDAO();
    
    virtual ~FrndLstDAO();


};

#endif
