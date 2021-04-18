#include <FrndLstDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>
#include <Constants.h>

FrndLst::FrndLst():m_bUpdAutoSpree(false), m_bUpdOpenHouses(false), m_bUpdEasyList(false), m_bUpdNShareList(false)
{

}

FrndLstDAO::FrndLstDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/frndLstRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open frndLstRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}
}

FrndLstDAO::~FrndLstDAO()
{
    delete m_db;
}


bool 
FrndLstDAO::store(long shareId, const FrndLst& frndLst)
{
    std::stringstream key;
    key << shareId;
    std::stringstream value;
   
    if (frndLst.m_bUpdAutoSpree)
        value << "YES|";
    else 
        value << "NO|";
    
    if (frndLst.m_bUpdOpenHouses)
        value << "YES|";
    else 
        value << "NO|";
    
    if (frndLst.m_bUpdEasyList)
        value << "YES|";
    else 
        value << "NO|";

    if (frndLst.m_bUpdNShareList)
        value << "YES}";
    else 
        value << "NO}";

    for (const auto& contact : frndLst.m_bFrndLst)
    {
        std::string frndName = contact.name;
        frndName.erase(std::remove(frndName.begin(), frndName.end(), '|'), frndName.end());
        value << frndName << "|" << contact.shareId << "|";

    }
    
    rocksdb::Status status;
    status  = m_db->Put(rocksdb::WriteOptions(), key.str(), value.str());
    if (status.ok())
    {
        std::cout << Util::now() << "Stored friend list shareId=" << shareId << " list=" << value.str() << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    }
    else
    {
        std::cout << Util::now() << "Failed to store friend list shareId=" << shareId << " list=" << value.str() << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    
    return true;
}

bool 
FrndLstDAO::get(long shareId, FrndLst& frndLst)
{

    rocksdb::Status status;
    std::string frndLstStr;
    std::stringstream key;
    key << shareId;
    status = m_db->Get(rocksdb::ReadOptions(), key.str(), &frndLstStr);
    if (status.ok())
    {
        std::cout << Util::now() << "Retrieved frndLst=" << frndLstStr << " for shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    }
    else
    {
        std::cout << Util::now() << "Failed to retrieve friend list for shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;
        return false;
    }

    auto valVec = Util::split(frndLstStr, '}');
    if (!valVec.size())
    {
        std::cout << Util::now() << "Invalid format for frndLst" << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }

    auto updateVec = Util::split(valVec[0], '|');
   
    if (updateVec[0] == "NO")
        frndLst.m_bUpdAutoSpree = false;
    else
        frndLst.m_bUpdAutoSpree = true;
    
    if (updateVec[1] == "NO")
        frndLst.m_bUpdOpenHouses = false;
    else
        frndLst.m_bUpdOpenHouses = true;

    if (updateVec[2] == "NO")
        frndLst.m_bUpdEasyList = false;
    else
        frndLst.m_bUpdEasyList = true;
    if (updateVec.size() > 3)
    {
        if (updateVec[3] == "NO")
            frndLst.m_bUpdNShareList = false;
        else
            frndLst.m_bUpdNShareList = true;
    }

    if (valVec.size() < 2)
    {
        std::cout << Util::now() << "Empty friend list" << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        return false;
    }
    
    auto frndLstVec = Util::split(valVec[1], '|');

    Frnd frnd;
    int i=0;
    for (const auto& elem : frndLstVec)
    {
       if (i%2)
       {
            frnd.shareId = std::stol(elem); 
            frndLst.m_bFrndLst.push_back(frnd);
       }
       else
       {
            frnd.name = elem;        
       }
       ++i; 
    }    
    return true;
}

bool 
FrndLstDAO::del(long shareId)
{
    std::stringstream key;
    key << shareId;
    rocksdb::Status status;
    status = m_db->Delete(rocksdb::WriteOptions(), key.str());
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to delete friend list for shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;  
        return false;
    }
    std::cout << Util::now() << "Deleted friend list for shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;  
    return true;
}

bool 
FrndLstDAO::update(long shareId, const FrndLst& frndLst)
{
    del(shareId);
    store(shareId, frndLst);
    return true;
}
