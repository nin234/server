#include <Distributor.h>
#include <Config.h>
#include <chrono>

using namespace std::chrono_literals;

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
    createAndSendMsgs(hostPortShareIds, pLstObj);
    return lclShareIds;
}

void 
Distributor::createAndSendMsgs(std::map<std::pair<std::string, int>, 
                std::vector<std::string>>& hostPortShareIds, LstObj *pLstObj)
{
    for (auto [hostPort, shareIds] : hostPortShareIds)
    {
        auto [host, port] = hostPort;
        DistribItem shareItem;
        shareItem.host = host;
        shareItem.port = port;
    
        if (m_pTrnsl->createShareItemMsg(shareItem.msg, pLstObj, shareIds))
        {
            std::lock_guard<std::mutex> lock(m_shareItemsMutex);
            m_shareItems.push_back(shareItem);
            m_shareItemsCV.notify_all();
        }
        //m_ntwIntf.sendMsg(buf, mlen, host, port);
    }
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

void*
Distributor::entry(void *context)
{
    return reinterpret_cast<Distributor*>(context)->main();    
}

void*
Distributor::main()
{

    for (;;)
    {
        std::unique_lock<std::mutex> lock(m_shareItemsMutex);
        m_shareItemsCV.wait_for(lock, 5s);
        if (m_shareItems.size())
        {
            for (auto& shareItem : m_shareItems)
            {
                m_ntwIntf.sendMsg(shareItem.msg.data(), shareItem.msg.size(), shareItem.host, shareItem.port);
            }
            m_shareItems.clear();
        }
    }       
    return this;
}


