#ifndef _DISTRIB_MGR_H_
#define _DISTRIB_MGR_H_

#include <string>
#include <map>

class DistribMgr
{

        int getAppId(const std::string& name);
        std::map<std::string, int> m_appIdMp;
    public:

        DistribMgr();
        virtual ~DistribMgr();
        static DistribMgr& Instance();
        bool storeNodeInfo();
        std::pair<std::string, int> getNewShareIdHost();
};

#endif
