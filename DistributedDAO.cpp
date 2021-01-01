#include <DistributedDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>
#include <memory>

DistributedDAO::DistributedDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/distributedRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open distributedRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}
	
}

DistributedDAO::~DistributedDAO()
{
    delete m_db;
}

bool
DistributedDAO::setNode(int startShareId, int endShareId, const std::map<int, std::pair<std::string, int>>& hostPortMap)
{
    std::stringstream key;
    key << startShareId;
    rocksdb::Status status;
    std::stringstream value;
    value << endShareId; 
    for (const auto [appId, hostPort] : hostPortMap)
    {
        auto [host, port] = hostPort;
        value << "|" << appId << "|" << host <<"|"  << port;
    }

    status = m_db->Put(rocksdb::WriteOptions(), key.str(), value.str());
    if (status.ok())
    {
        std::cout << Util::now() << "Stored Item to Distributed DB startShareId=" << startShareId << " value=" << value.str() << " " << __FILE__ << ":" << __LINE__ << std::endl;   
    }
    else
    {
        std::cout << Util::now() << "Failed store Item to Distributed DB startShareId=" << startShareId << " value=" << value.str() << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        return false;
    }
    return true;
}

long
DistributedDAO::getEndShareId()
{
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    pItr->SeekToLast();
    if (pItr->Valid())
    {
        std::string value = pItr->value().ToString();
        auto valVec = Util::split(value, '|');
        if (valVec.size())
            return std::stol(valVec[0]);
    }
    return 0;
}

bool
DistributedDAO::getLastNode(int appId, long& startShareId, std::pair<std::string, int>& hostPort)
{
    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    pItr->SeekToLast();
    if (pItr->Valid())
    {
        std::string value = pItr->value().ToString();
        auto valVec = Util::split(value, '|');
        startShareId = std::stol(pItr->key().ToString());
        for (unsigned long i=1; i < valVec.size(); i+=3)
        {
            if (appId == std::stoi(valVec[i]))
            {
                hostPort.first = valVec[i+1];
                hostPort.second = std::stoi(valVec[i+2]); 
                std::cout << Util::now() << "Distributed Node for appId=" << appId  << " host="<< hostPort.first << " port=" << hostPort.second<< " " << __FILE__ << ":" << __LINE__ << std::endl; 
                return true;
            }
        }        
            
    }
    else
    {
        std::cout << Util::now() << "Failed to find distributed Node for appId=" << appId  << " " << __FILE__ << ":" << __LINE__ << std::endl; 
    }
    return false;
}
bool
DistributedDAO::isNode(int shareId)
{
    std::stringstream key;
    key << shareId;
    rocksdb::Status status;
    std::string node;
    status = m_db->Get(rocksdb::ReadOptions(), key.str(), &node);
    if (!status.ok())
    {
        std::cout << Util::now() << "Failed to find shareId=" << shareId << " in distributed DB" << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    std::cout << Util::now() << "Found shareId=" << shareId << " in distributed DB" << " " << __FILE__ << ":" << __LINE__ << std::endl;    
    return true;
}


bool
DistributedDAO::getNode(int shareId, int appId, std::pair<std::string, int>& hostPort)
{

    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    std::stringstream key;
    key << shareId; 
    pItr->SeekForPrev(key.str());
    if (pItr->Valid())
    {
        std::string value = pItr->value().ToString();
        auto valVec = Util::split(value, '|');
        for (unsigned long i=1; i < valVec.size(); i+=3)
        {
            if (appId == std::stoi(valVec[i]))
            {
                hostPort.first = valVec[i+1];
                hostPort.second = std::stoi(valVec[i+2]); 
                std::cout << Util::now() << "Distributed Node for appId=" << appId << " shareId=" << shareId << " host="<< hostPort.first << " port=" << hostPort.second<< " " << __FILE__ << ":" << __LINE__ << std::endl; 
                return true;
            }
        }        
            
    }
    else
    {
        std::cout << Util::now() << "Failed to find distributed Node for appId=" << appId << " shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl; 
    }
    return false;
}
