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

    bool setNode(int startShareId, int endShareId, const std::map<int, std::pair<std::string, int>>& hostPortMp);

    bool getNode(int shareId, int appId, std::pair<std::string, int>& hostPort);
    bool getLastNode(int appId, std::pair<std::string, int>& hostPort);

    bool isNode(int shareId);    
};

#endif
