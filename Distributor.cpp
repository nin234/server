#include <Distributor.h>
#include <DistribMgr.h>
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



std::vector<std::string>
Distributor::distribute(std::shared_ptr<LstObj> pLstObj)
{

    std::vector<std::string> shareIds;

    std::vector<std::string> lclShareIds;

    if (m_pTrnsl->getShareIds(pLstObj->getList(), shareIds))
    {
        for (auto shareIdStr : shareIds)
        {
            long nShareid = std::stol(shareIdStr);
            if (nShareid >= Config::Instance().getStartShareId() && nShareid <= Config::Instance().getEndShareId())
            {
                lclShareIds.push_back(shareIdStr);
            }
        }
        std::lock_guard<std::mutex> lock(m_shareItemsMutex);
        m_shareItems.push_back(pLstObj);
        m_shareItemsCV.notify_all();
    }
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
Distributor::createAndSendMsgs(std::map<std::pair<std::string, int>, 
                std::vector<std::string>>& hostPortShareIds, 
                std::shared_ptr<LstObj> pLstObj)
{
    std::vector<std::string> failedToSendShareIds;    
    for (auto [hostPort, shareIds] : hostPortShareIds)
    {
        auto [host, port] = hostPort;
        DistribItem shareItem;
        shareItem.host = host;
        shareItem.port = port;
        shareItem.pLstObj = pLstObj; 

        if (m_pTrnsl->createShareItemMsg(shareItem.msg, pLstObj, shareIds))
        {

            if (!m_ntwIntf.sendMsg(shareItem.msg.data(), shareItem.msg.size(), shareItem.host, shareItem.port))
            {
                failedToSendShareIds.insert(failedToSendShareIds.end(), shareIds.begin(), shareIds.end()); 
            }
        }
    }
    DistribMgr::Instance().del(pLstObj);
    if (!failedToSendShareIds.size())
    {
        return;
    }
    if (m_pTrnsl->changeShareIds(pLstObj, failedToSendShareIds))
    {
        DistribMgr::Instance().store(pLstObj);
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
        if (DistribMgr::Instance().getNode(shareId, appId, hostPort))
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
Distributor::getRemoteShareIds(std::shared_ptr<LstObj> pLstObj, 
                                std::vector<std::string>& remoteShareIds)
{
    std::vector<std::string> shareIds;
    if (m_pTrnsl->getShareIds(pLstObj->getList(), shareIds))
    {
        for (auto shareIdStr : shareIds)
        {
            long nShareid = std::stol(shareIdStr);
            if (nShareid < Config::Instance().getStartShareId() || nShareid > Config::Instance().getEndShareId())
            {
                remoteShareIds.push_back(shareIdStr);
            }
        }
    }
}

void
Distributor::processShareItem(std::shared_ptr<LstObj> pLstObj)
{
    std::vector<std::string> remoteShareIds;
    
    getRemoteShareIds(pLstObj, remoteShareIds);    

    if (!remoteShareIds.size())
    {
        return;
    }
    DistribMgr::Instance().store(pLstObj);
    
    std::map<std::pair<std::string, int>, std::vector<std::string>> hostPortShareIds;
    populateShareIdHostMap(pLstObj->getAppId(), hostPortShareIds, remoteShareIds);
    createAndSendMsgs(hostPortShareIds, pLstObj);
}

void
Distributor::processShareItems()
{
        if (m_shareItemsLcl.size())
        {
            for (auto& shareItem : m_shareItemsLcl)
            {
                processShareItem(shareItem);
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
                std::vector<std::string> remoteShareIds;
                getRemoteShareIds(picture, remoteShareIds);    
                if (!remoteShareIds.size())
                {
                    continue;
                }
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
            std::vector<std::string> remoteShareIds;
            getRemoteShareIds(pPicMetaObj, remoteShareIds);    
            if (!remoteShareIds.size())
            {
                continue;
            }
            DistribMgr::Instance().store(pPicMetaObj);
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
        if (!DistribMgr::Instance().get(file, host, port, bSend))
        {
            bSend = false;
            DistribMgr::Instance().store(file, host, port, bSend);
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
                DistribMgr::Instance().del(pPicMetaObj);
                DistribMgr::Instance().store(file, host, port, true);
            }
            else
            {
                std::cout << Util::now() << "Failed to Sent picture=" << *pPicMetaObj << " host=" << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;   
                return false;

            }
        }
        else
        {
            DistribMgr::Instance().del(pPicMetaObj);
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

    auto shareItems = DistribMgr::Instance().getAllShareItems();
    
    for (auto shareItem : shareItems)
    {
        processShareItem(shareItem);
    }

    auto picMetaDatas = DistribMgr::Instance().getAll();

    for (auto picMetaData : picMetaDatas)
    {
        checkPictureAndProcess(picMetaData);
    }
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
    DistribMgr::Instance().getAll(file, picNodes);
	int fd  = -1;
	fd = open(file.c_str(), O_RDONLY);
	
	if (fd == -1)
	{
		std::cout << "Failed to open file " << file  << " " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return;
	}
	std::cout << "Opened file=" << file << " fd=" << fd << " to distribute picture " << __FILE__ << ":" << __LINE__ << std::endl;

    for (;;)
    {
        char buf[MAX_BUF];
	    int msglen; 
        if (!m_pTrnsl->getPicMsg(fd, pPicMetaObj->getAppId(), buf, &msglen))
        {
            close(fd);
            break;
        }
 
        for (const auto& picNode : picNodes)
        {
            if (!picNode.send)
            {
                m_ntwIntf.sendMsg(buf, msglen, picNode.host, picNode.port);
            }
        }
    }
}

