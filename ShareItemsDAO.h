#ifndef _SHAREITEMSDAO_H_
#define _SHAREITEMSDAO_H_

#include <rocksdb/db.h>

class ShareItemsDAO
{

	rocksdb::DB *m_db;

    public:

        ShareItemsDAO();

        virtual ~ShareItemsDAO();

        void storeItem(int appId, long shareIdLst, const std::string& name, const std::string& list, const std::vector<std::string>& shareIds);
};
#endif
