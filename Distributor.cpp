#include <Distributor.h>
#include <Config.h>

Distributor::Distributor()
{

}

Distributor::~Distributor()
{

}

std::vector<std::string>
Distributor::distribute(LstObj *pLstObj)
{

    std::vector<std::string> shareIds;

    std::vector<std::string> lclShareIds;
    std::vector<std::string> remoteShareIds;

    if (m_pTrnsl->getShareIds(pLstObj->getList(), shareIds))
    {
        for (auto shareIdStr : shareIds)
        {
            long nShareid = std::stol(shareIdStr);
            if (nShareid >= Config::Instance().getStartShareId() && nShareid <= Config::Instance().getEndShareId())
            {
                lclShareIds.push_back(shareIdStr);
            }
            else
            {
                remoteShareIds.push_back(shareIdStr);
            }
        }
    }
    std::map<std::pair<std::string, int>, std::vector<std::string>> hostPortShareIds;
    populateShareIdHostMap(pLstObj->getAppId(), hostPortShareIds, remoteShareIds);
    return lclShareIds;
}

void
Distributor::setTrnsl(MessageTranslator *pTrnsl)
{
	m_pTrnsl.reset(pTrnsl);
	return;
}


void 
Distributor::populateShareIdHostMap(int appId, std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::vector<std::string>& remoteShareIds)
{
    for (auto rShareIdStr : remoteShareIds)
    {
        long shareId = std::stol(rShareIdStr); 
        std::pair<std::string, int> hostPort;
        if (m_distribDAO.getNode(shareId, appId, hostPort))
        {
            hostPortShareIds[hostPort].push_back(rShareIdStr);
        }
        
    }
}
