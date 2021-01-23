#include <Distributor.h>
#include <Config.h>
#include <chrono>
#include <Util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Constants.h>

using namespace std::chrono_literals;

Distributor::Distributor()
{

}

Distributor::~Distributor()
{

}


void
Distributor::processPicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{

    std::vector<std::string> lclShareIds;
    std::vector<std::string> remoteShareIds;

    auto& shareIds = pPicMetaObj->getFrndLst(); 
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
    
    std::map<std::pair<std::string, int>, std::vector<std::string>> hostPortShareIds;
    populateShareIdHostMap(pPicMetaObj->getAppId(), hostPortShareIds, remoteShareIds);
    createAndSendMsgs(hostPortShareIds, pPicMetaObj);
    storePicDistribInfo(hostPortShareIds, pPicMetaObj);

    return;

}

std::vector<std::string>
Distributor::distribute(std::shared_ptr<LstObj> pLstObj)
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
Distributor::distribute(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
        std::lock_guard<std::mutex> lock(m_shareItemsMutex);
        m_picMetaDatas.push_back(pPicMetaObj);
        m_shareItemsCV.notify_all();
}

void
Distributor::distributePicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
        std::lock_guard<std::mutex> lock(m_shareItemsMutex);
        m_pictures.push_back(pPicMetaObj);
        m_shareItemsCV.notify_all();
}

void 
Distributor::storePicDistribInfo(std::map<std::pair<std::string, int>, 
                std::vector<std::string>>& hostPortShareIds, std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{

    for (auto [hostPort, shareIds] : hostPortShareIds)
    {
        auto [host, port] = hostPort;
        std::string picUrl = Util::constructPicFile(pPicMetaObj->getShrId(), pPicMetaObj->getAppId(), pPicMetaObj->getName());
        bool bSend;
        if (!m_picDistribDAO.get(picUrl, host, port, bSend))
        {
            bSend = false;
            m_picDistribDAO.store(picUrl, host, port, bSend);
        }
        
    }
}

void 
Distributor::createAndSendMsgs(std::map<std::pair<std::string, int>, 
                std::vector<std::string>>& hostPortShareIds, std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
    for (auto [hostPort, shareIds] : hostPortShareIds)
    {
        auto [host, port] = hostPort;
        DistribItem shareItem;
        shareItem.host = host;
        shareItem.port = port;
    
        if (m_pTrnsl->createServerPicMetaMsg(shareItem.msg, pPicMetaObj, shareIds))
        {
            std::lock_guard<std::mutex> lock(m_shareItemsMutex);
            m_shareItems.push_back(shareItem);
            m_shareItemsCV.notify_all();
        }
    }
}


void 
Distributor::createAndSendMsgs(std::map<std::pair<std::string, int>, 
                std::vector<std::string>>& hostPortShareIds, 
                std::shared_ptr<LstObj> pLstObj)
{
    for (auto [hostPort, shareIds] : hostPortShareIds)
    {
        auto [host, port] = hostPort;
        DistribItem shareItem;
        shareItem.host = host;
        shareItem.port = port;
        shareItem.pLstObj = pLstObj; 

        if (m_pTrnsl->createShareItemMsg(shareItem.msg, pLstObj, shareIds))
        {
            std::lock_guard<std::mutex> lock(m_shareItemsMutex);
            m_shareItems.push_back(shareItem);
            m_shareItemsCV.notify_all();
        }
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

void
Distributor::waitAndCopyItems()
{
    std::unique_lock<std::mutex> lock(m_shareItemsMutex);
    m_shareItemsCV.wait_for(lock, 5s);
    m_shareItemsLcl.splice(m_shareItemsLcl.end(),m_shareItems);
    m_picturesLcl.splice(m_picturesLcl.end(),  m_pictures);
    m_picMetaDatasLcl.splice(m_picMetaDatasLcl.end() , m_picMetaDatas);
}

void
Distributor::processShareItems()
{
        if (m_shareItemsLcl.size())
        {
            for (auto shareItem  = m_shareItemsLcl.begin(); shareItem != m_shareItemsLcl.end();)
            {
                if (m_ntwIntf.sendMsg(shareItem->msg.data(), shareItem->msg.size(), shareItem->host, shareItem->port))
                {
                    shareItem = m_shareItemsLcl.erase(shareItem);
                }
                else
                {
                    ++shareItem;
                }
            }
        }
}

void*
Distributor::main()
{

    for (;;)
    {
        waitAndCopyItems();
        processShareItems(); 

        if (m_pictures.size())
        {
            for (auto picture : m_pictures)
            {
                sendPicture(picture);
            }
        }
    }       
    return this;
}

void
Distributor::sendPicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
     
	std::string file = Util::constructPicFile(pPicMetaObj->getShrId(), pPicMetaObj->getAppId(), pPicMetaObj->getName());
    if (!file.size())
	{
		std::cout << "Invalid picture file shareId=" << pPicMetaObj->getShrId() << " appId=" << pPicMetaObj->getAppId() << " name=" << pPicMetaObj->getName() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return;
	}

    std::vector<PicNode> picNodes;
    m_picDistribDAO.getAll(file, picNodes);
	int fd  = -1;
	fd = open(file.c_str(), O_RDONLY);
	
	if (fd == -1)
	{
		std::cout << "Failed to open file " << file  << " " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return;
	}
	std::cout << "Opened file=" << file << " to distribute picture " << __FILE__ << ":" << __LINE__ << std::endl;

    for (;;)
    {
        char buf[MAX_BUF];
		constexpr int msgId = PIC_MSG;
		memcpy(buf+sizeof(int), &msgId, sizeof(int));		
        
		int numread = read(fd, buf+2*sizeof(int), MAX_BUF-2*sizeof(int));
		if (!numread ||  numread == -1)
		{
			std::cout << "Finished reading file " << file << " numread=" << numread << " "  << __FILE__ << ":" << __LINE__ << std::endl;
		    close(fd);
			break;
		}
		int msglen = numread + 2*sizeof(int);
		memcpy(buf, &msglen, sizeof(int));
        
        for (const auto& picNode : picNodes)
        {
            if (!picNode.send)
            {
                m_ntwIntf.sendMsg(buf, msglen, picNode.host, picNode.port);
            }
        }
    }
}

