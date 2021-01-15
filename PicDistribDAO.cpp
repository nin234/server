#include <PicDistribDAO.h>
#include <iostream>
#include <sstream>
#include <Util.h>

PicDistribDAO::PicDistribDAO()
{
   	rocksdb::Options options;
	options.create_if_missing = true;
	rocksdb::Status  status = rocksdb::DB::Open(options, "/home/ninan/data/picDistribRocks", &m_db);
	if (!status.ok())
	{
		std::cout << "Failed to open picDistribRocks database, exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		exit(0);
	}

}

PicDistribDAO::~PicDistribDAO()
{
    delete m_db;
}
  
bool 
PicDistribDAO::store(const std::string& picUrl, const std::string& host, int port,
               bool bSend)
{
    std::stringstream key;
    key << picUrl << "|" << host << "|" << port;
    rocksdb::Status status;
    std::string value = "NO";
    if (bSend)
    {
        value = "YES";
    }
    status = m_db->Put(rocksdb::WriteOptions(), key.str(), value);
    if (status.ok())
    {
        std::cout << Util::now() << "Stored picture distribute picUrl=" << picUrl << " host=" << host << " port=" << port << " send=" << value << " " << __FILE__ << ":" << __LINE__ << std::endl; 
    }
    else
    {
        std::cout << Util::now() << "Failed to store  picture distribute picUrl=" << picUrl << " host=" << host << " port=" << port << " send=" << value << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        return false;

    }

    return true;
}

bool 
PicDistribDAO::get(const std::string& picUrl, const std::string& host, int port,
               bool& bSend)
{
    bSend = false; 
    std::stringstream key;
    key << picUrl << "|" << host << "|" << port;
    rocksdb::Status status;
    std::string value;
    status = m_db->Get(rocksdb::ReadOptions(), key.str(), &value);
    if (status.ok())
    {
        if (value == "YES")
            bSend = true;
        std::cout << Util::now() << "Retrieved picture distribute picUrl=" << picUrl << " host=" << host << " port=" << port << " send=" << value << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        
    }
    else
    {
        std::cout << Util::now() << "Failed to retrieve picture distribute picUrl=" << picUrl << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        return false;

    }
    return true;
}

bool
PicDistribDAO::getAll(const std::string& picUrl, std::vector<PicNode>& picNodes)
{

    std::unique_ptr<rocksdb::Iterator> pItr(m_db->NewIterator(rocksdb::ReadOptions()));
    std::stringstream key;
    key << picUrl;

    std::string prefix = key.str();
    for (pItr->Seek(prefix); pItr->Valid();
       pItr->Next()) 
    {
        std::string keyStr = pItr->key().ToString();
        if (keyStr.rfind(prefix, 0) == 0)
        {
           auto keyVec = Util::split(keyStr, '|');
           if (keyVec.size() != 3)
           {
                std::cout << Util::now() << "Invalid size for keyVec=" << keyVec.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;    
                continue;
           }
           PicNode picNode;
           picNode.host = keyVec[1];
           picNode.port = std::stoi(keyVec[2]);
           std::string val = pItr->value().ToString();
           picNode.send = false;
           if (val == "YES")
           {
                picNode.send = true;
           }
           picNodes.push_back(picNode);
        } 
        else
        {
            break;
        }
    }
    return true;
}
