#ifndef _DISTRIB_MGR_H_
#define _DISTRIB_MGR_H_

#include <string>
#include <map>
#include <DistributedDAO.h>

class DistribMgr
{
        DistributedDAO m_distribDAO;
        int getAppId(const std::string& name);
        std::map<std::string, int> m_appIdMp;
    public:

        DistribMgr();
        virtual ~DistribMgr();
        static DistribMgr& Instance();
        bool storeNodeInfo();
        std::pair<std::string, int> getNewShareIdHost(int appId);
        long getEndShareId();
        bool getNode(int shareId, int appId, std::pair<std::string, int>& hostPort);
        
};

#endif
