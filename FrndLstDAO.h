#ifndef _FRNDLSTDAO_H_
#define _FRNDLSTDAO_H_

#include <rocksdb/db.h>
#include <vector>


struct Frnd
{
    std::string name;
    long        shareId;    
};

struct FrndLst
{
    bool m_bUpdAutoSpree;
    bool m_bUpdOpenHouses;
    bool m_bUpdEasyList; 

    std::vector<Frnd> m_bFrndLst;
    
    FrndLst();
};

class FrndLstDAO
{
    rocksdb::DB *m_db;

    public:

    FrndLstDAO();
    
    virtual ~FrndLstDAO();

    bool store(long shareId, const FrndLst& frndLst);

    bool get(long shareId, FrndLst& frndLst);

    bool del(long shareId);

    bool update(long shareId, const FrndLst& frndLst);

};

#endif
