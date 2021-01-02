#include <DistribMgr.h>
#include <Config.h>
#include <DistributedDAO.h>
#include <iostream>
#include <Constants.h>

DistribMgr::DistribMgr()
{
    m_appIdMp["AutoSpree"] = AUTOSPREE_ID;
    m_appIdMp["OpenHouses"] = OPENHOUSES_ID;
    m_appIdMp["EasyGrocList"] = EASYGROCLIST_ID;
}


DistribMgr::~DistribMgr()
{

}

int 
DistribMgr::getAppId(const std::string& name)
{
    auto pItr = m_appIdMp.find(name);
    if (pItr == m_appIdMp.end())
        return -1;
    return pItr->second;
}

DistribMgr&
DistribMgr::Instance()
{
    static DistribMgr instance;
    return instance;
}

std::pair<std::string, int>
DistribMgr::getNewShareIdHost(int appId)
{
    std::pair<std::string, int> hostPort;
    long startShareId;
    if (m_distribDAO.getLastNode(appId, startShareId, hostPort))
    {
        if (Config::Instance().getStartShareId() == startShareId)
        {
            return {"LOCAL", 0};
        }
        return hostPort;
    }
    return {"LOCAL", 0};
}
bool
DistribMgr::storeNodeInfo()
{
    int startShareId = Config::Instance().getStartShareId();
    if (m_distribDAO.isNode(startShareId))
    {
        std::cout << "startShareId=" << startShareId << " already in distributed DB " << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        return true;
    }
    int endShareId = Config::Instance().getEndShareId();
    auto appConns = Config::Instance().getAppConns();
    std::map<int, std::pair<std::string, int>> appHostPortMp;
    for (auto [appName, hostPort] : appConns)
    {
        int appId = getAppId(appName); 
        if (appId == -1)
        {
            std::cout << "Failed to find appId for appName=" << appName << " exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;   
            exit(0);
        }
        appHostPortMp[appId] = hostPort;
    }
    m_distribDAO.setNode(startShareId, endShareId, appHostPortMp);
    return true;
}

long
DistribMgr::getEndShareId()
{

    return m_distribDAO.getEndShareId();
}