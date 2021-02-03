#include <DistribMgr.h>
#include <Config.h>
#include <DistributedDAO.h>
#include <iostream>
#include <Constants.h>
#include <tinyxml2.h>
#include <stdexcept>

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
   	tinyxml2::XMLDocument doc;
	if (doc.LoadFile("/home/ninan/config/config.xml") != tinyxml2::XML_SUCCESS)
	{
		std::cout << "Failed to parse xml file " << std::endl;
		throw std::runtime_error("Failed to parse xml file /home/ninan/config/config.xml");
	}
	for (auto oh = doc.FirstChildElement()->FirstChildElement();oh != NULL; oh = oh->NextSiblingElement())
	{
        auto apps = oh->FirstChildElement("Apps");
        std::map<int, std::pair<std::string, int>> appHostPortMp;

        for (auto app = apps->FirstChildElement(); app != NULL; app = app->NextSiblingElement())
        {
            auto name = app->FirstChildElement("name");
            auto host = app->FirstChildElement("host");
            auto port = app->FirstChildElement("port");
            std::string nameStr = name->GetText();
            std::string hostStr = host->GetText();
            int portVal = std::stoi(port->GetText());
            std::cout << "Adding host details name=" << nameStr << " port=" << portVal << " host=" << hostStr << " " << __FILE__ << ":" << __LINE__ << std::endl;          
            int appId = getAppId(nameStr); 
            if (appId == -1)
            {
                std::cout << "Failed to find appId for appName=" << nameStr << " exiting" << " " << __FILE__ << ":" << __LINE__ << std::endl;   
                exit(0);
            }
            appHostPortMp[appId] = std::make_pair(hostStr, portVal); 
        }
        auto shareIds = doc.FirstChildElement()->FirstChildElement("ShareIds");

        auto start = shareIds->FirstChildElement("start");
        auto startShareId = std::stoi(start->GetText());
        auto end = shareIds->FirstChildElement("end");
        auto endShareId = std::stoi(end->GetText());
        m_distribDAO.setNode(startShareId, endShareId, appHostPortMp);
    }
    return true;
}

long
DistribMgr::getEndShareId()
{

    return m_distribDAO.getEndShareId();
}
