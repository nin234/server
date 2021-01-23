#ifndef _SHAREITEMSDISTRIBDAO_H_
#define _SHAREITEMSDISTRIBDAO_H_

#include <rocksdb/db.h>
#include <MessageObjs.h>
#include <memory>

class ShareItemsDistribDAO
{

	rocksdb::DB *m_db;

    public:

        ShareItemsDistribDAO();
            
        virtual ~ShareItemsDistribDAO();

        bool store(const std::string& host, int port, std::shared_ptr<LstObj> pLstObj);
        bool del(const std::string& host, int port, std::shared_ptr<LstObj> pLstObj);

};

#endif
