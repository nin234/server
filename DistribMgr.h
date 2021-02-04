#ifndef _DISTRIB_MGR_H_
#define _DISTRIB_MGR_H_

#include <string>
#include <map>
#include <DistributedDAO.h>
#include <PicDistribDAO.h>
#include <PicMetaDistribDAO.h>
#include <ShareItemsDistribDAO.h>

class DistribMgr
{
        DistributedDAO m_distribDAO;
        int getAppId(const std::string& name);
        std::map<std::string, int> m_appIdMp;
        PicDistribDAO m_picDistribDAO;
        PicMetaDistribDAO m_picMetaDistribDAO;
        ShareItemsDistribDAO m_shareItemsDAO;

    public:

        DistribMgr();
        virtual ~DistribMgr();
        static DistribMgr& Instance();
        bool storeNodeInfo();
        std::pair<std::string, int> getNewShareIdHost(int appId);
        long getEndShareId();
        bool getNode(int shareId, int appId, std::pair<std::string, int>& hostPort);
       
        bool store(const std::string& picUrl, const std::string& host, int port,
                   bool bSend);
        bool get(const std::string& picUrl, const std::string& host, int port,
                   bool& bSend);

        bool getAll(const std::string& picUrl, std::vector<PicNode>& picNodes);    
        
        bool store(std::shared_ptr<PicMetaDataObj> pPicMetaObj);

        bool del(std::shared_ptr<PicMetaDataObj> pPicMetaObj);

        std::list<std::shared_ptr<PicMetaDataObj>> getAll();
        
        bool store(std::shared_ptr<LstObj> pLstObj);

        bool del(std::shared_ptr<LstObj> pLstObj);

        std::list<std::shared_ptr<LstObj>> getAllShareItems();
};

#endif
