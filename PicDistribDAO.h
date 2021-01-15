#ifndef _PICDISTRIBDAO_H_
#define _PICDISTRIBDAO_H_

#include <rocksdb/db.h>
#include <string>
#include <vector>

struct PicNode
{
    std::string host;
    int port;
    bool send;
};

class PicDistribDAO
{
    rocksdb::DB *m_db;

  public:

    PicDistribDAO();
    virtual ~PicDistribDAO();

    bool store(const std::string& picUrl, const std::string& host, int port,
               bool bSend);
    bool get(const std::string& picUrl, const std::string& host, int port,
               bool& bSend);

    bool getAll(const std::string& picUrl, std::vector<PicNode>& picNodes);    
};

#endif
