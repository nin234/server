#ifndef _PICMETAROCKSDAO_H_
#define _PICMETAROCKSDAO_H_

#include <rocksdb/db.h>
#include <string>
#include <vector>
#include <StorageElems.h>

//RocksDB interface for storing picture metadata information
class PicMetaRocksDAO
{
    rocksdb::DB *m_db;

    public:

        PicMetaRocksDAO();

        virtual ~PicMetaRocksDAO();

        void store(int appId, 
                 long shareIdSender, 
                 const std::string& picName,
                 long picLen,
                const std::vector<std::string>& shareIdsRecvs);

        void get(int appId, long shareId, std::vector<shrIdLstName>& picNamesShIds);
        void del(int appId, 
                 long shareIdSender, 
                 const std::string& picName,
                 long shareIdRecv);
};

#endif
