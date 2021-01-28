#include <Distributor.h>
#include <Config.h>
#include <chrono>
#include <Util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <Constants.h>

using namespace std::chrono_literals;

Distributor::Distributor() : m_lastCheckTime(0)
{

}

Distributor::~Distributor()
{

}


void 
Distributor::getRemoteShareIds(std::shared_ptr<PicMetaDataObj> pPicMetaObj, 
                                std::vector<std::string>& remoteShareIds)
{
    std::vector<std::string> lclShareIds;

    auto& shareIds = pPicMetaObj->getFrndLst(); 
    for (auto shareIdStr : shareIds)
    {
        long nShareid = std::stol(shareIdStr);
        if (nShareid < Config::Instance().getStartShareId() || nShareid > Config::Instance().getEndShareId())
        {
            remoteShareIds.push_back(shareIdStr);
        }
    }
 
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
            m_shareItemsDAO.store(host, port, pLstObj);
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
    m_shareItemsLcl = m_shareItems;
    m_picturesLcl = m_pictures;
    m_picMetaDatasLcl = m_picMetaDatas;
    m_shareItems.clear();
    m_picMetaDatas.clear();
    m_pictures.clear();
}

void
Distributor::processShareItems()
{
        if (m_shareItemsLcl.size())
        {
            for (auto& shareItem : m_shareItemsLcl)
            {
                if (m_ntwIntf.sendMsg(shareItem.msg.data(), shareItem.msg.size(), shareItem.host, shareItem.port))
                {
                    m_shareItemsDAO.del(shareItem.host, shareItem.port, shareItem.pLstObj);
                }
            }
        }
}

void
Distributor::processPictures()
{
        if (m_picturesLcl.size())
        {
            for (auto picture : m_picturesLcl)
            {
                checkPictureAndProcess(picture);
            }
        }
}

void
Distributor::processPicMetaDatas()
{
    if (m_picMetaDatasLcl.size())
    {
        for (auto pPicMetaObj : m_picMetaDatasLcl)
        {
            m_picMetaDistribDAO.store(pPicMetaObj);
            checkPictureAndProcess(pPicMetaObj);
        }
    }
}

void
Distributor::checkPictureAndProcess(std::shared_ptr<PicMetaDataObj> pPicMetaObj)
{
    std::string file = Util::constructPicFile(pPicMetaObj->getShrId(), pPicMetaObj->getAppId(), pPicMetaObj->getName());
    if (!file.size())
	{
		std::cout << Util::now() << "Invalid picture file shareId=" << pPicMetaObj->getShrId() << " appId=" << pPicMetaObj->getAppId() << " name=" << pPicMetaObj->getName() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return;
	}
	struct stat buf;
    if (stat(file.c_str(), &buf) == 0)
	{
		if (pPicMetaObj->getPicLen() > buf.st_size)
		{
            std::cout  << Util::now()<< "Waiting for picture to be uploaded " << *pPicMetaObj << " " << __FILE__ << ":" << __LINE__ << std::endl;  
			return;
		}
	}
    else
    {
        std::cout << Util::now() << "File not uploaded " << file << " error=" << errno << " for picmetadata" << *pPicMetaObj << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return;
    }

    std::vector<std::string> remoteShareIds;
    getRemoteShareIds(pPicMetaObj, remoteShareIds);    
    std::map<std::pair<std::string, int>, std::vector<std::string>> hostPortShareIds;
    populateShareIdHostMap(pPicMetaObj->getAppId(), hostPortShareIds, remoteShareIds);

    for (auto [hostPort, shareIds] : hostPortShareIds)
    {
        auto [host, port] = hostPort;
        bool bSend;
        if (!m_picDistribDAO.get(file, host, port, bSend))
        {
            bSend = false;
            m_picDistribDAO.store(file, host, port, bSend);
        }
        sendMetaDataAndIfReqdPic(file, pPicMetaObj, shareIds, host, port, !bSend);
    }
}

bool 
Distributor::sendMetaDataAndIfReqdPic(std::string file, std::shared_ptr<PicMetaDataObj> pPicMetaObj, const std::vector<std::string>& shareIds, const std::string& host, int port, bool both)
{
    if (sendPicMetaData(pPicMetaObj, shareIds, host, port))
    {
        std::cout << Util::now() << "Sent picture metadata=" << *pPicMetaObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        if (both)
        {
            if (sendPicture(pPicMetaObj, host, port))
            {
                std::cout << Util::now() << "Sent picture=" << *pPicMetaObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;   
                m_picMetaDistribDAO.del(pPicMetaObj);
                m_picDistribDAO.store(file, host, port, true);
            }
            else
            {
                std::cout << Util::now() << "Failed to Sent picture=" << *pPicMetaObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;   
                return false;

            }
        }
        else
        {
            m_picMetaDistribDAO.del(pPicMetaObj);
        }
    }
    else
    {
        std::cout << Util::now() << "Failed to sent picture metadata=" << *pPicMetaObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        return false;
    }
    return true;
}

bool 
Distributor::sendPicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj, const std::vector<std::string>& shareIds, const std::string& host, int port)
{
    std::vector<char> msg;
    if (m_pTrnsl->createServerPicMetaMsg(msg, pPicMetaObj, shareIds))
    {
        if (m_ntwIntf.sendMsg(msg.data(), msg.size(), host, port))
        {
            return true;
        }
    }
    return false;
}

void
Distributor::processArchivedItems()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
    {
        std::cout << "gettimeofday failed=" << errno << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return;
    }

    if (tv.tv_sec - m_lastCheckTime < Config::Instance().distribInterval())
        return;
    m_lastCheckTime = tv.tv_sec;
}

void*
Distributor::main()
{

    for (;;)
    {
        waitAndCopyItems();
        processShareItems(); 
        processPicMetaDatas();
        processPictures();
        processArchivedItems();
    }       
    return this;
}

bool
Distributor::sendPicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj, const std::string& host, int port)
{
     
	std::string file = Util::constructPicFile(pPicMetaObj->getShrId(), pPicMetaObj->getAppId(), pPicMetaObj->getName());
    if (!file.size())
	{
		std::cout << Util::now() << "Invalid picture file shareId=" << pPicMetaObj->getShrId() << " appId=" << pPicMetaObj->getAppId() << " name=" << pPicMetaObj->getName() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}

	int fd  = -1;
	fd = open(file.c_str(), O_RDONLY);
	
	if (fd == -1)
	{
		std::cout  << Util::now()<< "Failed to open file " << file  << " " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}
	std::cout  << Util::now()<< "Opened file=" << file << " to distribute picture " << __FILE__ << ":" << __LINE__ << std::endl;

    for (;;)
    {
        char buf[MAX_BUF];
		constexpr int msgId = PIC_MSG;
		memcpy(buf+sizeof(int), &msgId, sizeof(int));		
        
		int numread = read(fd, buf+2*sizeof(int), MAX_BUF-2*sizeof(int));
		if (!numread ||  numread == -1)
		{
			std::cout << Util::now() << "Finished reading file " << file << " numread=" << numread << " "  << __FILE__ << ":" << __LINE__ << std::endl;
		    close(fd);
			break;
		}
		int msglen = numread + 2*sizeof(int);
		memcpy(buf, &msglen, sizeof(int));
        if (!m_ntwIntf.sendMsg(buf, msglen, host, port))
        {
            std::cout  << Util::now()<< "Failed to send PIC_MSG to host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;  
            return false;
        }
        
    }

    return true;
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

