#ifndef _DISTRIBUTED_DAO_H_
#define _DISTRIBUTED_DAO_H_

#include <rocksdb/db.h>
#include <map>

class DistributedDAO
{

    rocksdb::DB *m_db;

    public:

    DistributedDAO();

    virtual ~DistributedDAO();

    bool setNode(int startShareId, int endShareId, const std::map<std::string, int>& hostPortMp);

    
};

#endif
