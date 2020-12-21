#ifndef _SHAREIDDAO_H_
#define _SHAREIDDAO_H_

#include <rocksdb/db.h>

class ShareIdDAO
{
    rocksdb::DB *m_db;

    public:

    ShareIdDAO();
    
    virtual ~ShareIdDAO();

    long get(const std::string& deviceId);
    
    bool set(const std::string& deviceId, long shareId);

    long getMaxShareId();

    bool setMaxShareId(long maxShareId);

};
#endif
