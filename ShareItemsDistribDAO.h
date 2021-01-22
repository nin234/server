#ifndef _SHAREITEMSDISTRIBDAO_H_
#define _SHAREITEMSDISTRIBDAO_H_

#include <rocksdb/db.h>

class ShareItemsDistribDAO
{

	rocksdb::DB *m_db;
    public:

        ShareItemsDistribDAO();
            
        virtual ~ShareItemsDistribDAO();
};

#endif
