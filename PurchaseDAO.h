#ifndef _PURCHASE_H_
#define _PURCHASE_H_

#include <rocksdb/db.h>

class PurchaseDAO
{
	rocksdb::DB *m_db;
	public:

    PurchaseDAO();
    virtual ~PurchaseDAO();

    void store(int appId, long shareId, const std::string& devId, const std::string& purchaseId);
    bool get(int appId, long shareId, const std::string& devId, std::string& purchaseId);
};

#endif
