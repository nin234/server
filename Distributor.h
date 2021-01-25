#ifndef _DISTRIBUTOR_H_
#define _DISTRIBUTOR_H_

#include <vector>
#include <string>
#include <MessageObjs.h>
#include <MessageTranslator.h>
#include <map>
#include <DistributedDAO.h>
#include <PicMetaDistribDAO.h>
#include <NtwClientIntf.h>
#include <pthread.h>
#include <list>
#include <mutex>
#include <condition_variable>
#include <PicDistribDAO.h>
#include <ShareItemsDistribDAO.h>

struct DistribItem
{
    std::string host;
    int port;
    std::vector<char> msg;
    std::shared_ptr<LstObj> pLstObj;
};

class Distributor
{

        std::shared_ptr<MessageTranslator> m_pTrnsl;
        DistributedDAO m_distribDAO;
        NtwClientIntf m_ntwIntf;
        PicDistribDAO m_picDistribDAO;
        PicMetaDistribDAO m_picMetaDistribDAO;
        std::mutex  m_shareItemsMutex;
        std::condition_variable m_shareItemsCV;        
        ShareItemsDistribDAO m_shareItemsDAO;

        std::list <DistribItem> m_shareItems;
        std::list<std::shared_ptr<PicMetaDataObj>> m_pictures;
        std::list<std::shared_ptr<PicMetaDataObj>> m_picMetaDatas;
        
        std::list <DistribItem> m_shareItemsLcl;
        std::list<std::shared_ptr<PicMetaDataObj>> m_picturesLcl;
        std::list<std::shared_ptr<PicMetaDataObj>> m_picMetaDatasLcl;

        void populateShareIdHostMap(int appId, std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::vector<std::string>& remoteShareIds);

        void createAndSendMsgs(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::shared_ptr<LstObj> pLsObj);

        void createAndSendMsgs(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::shared_ptr<PicMetaDataObj> pPicMetaObj);

        bool sendPicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj, const std::vector<std::string>& shareIds, const std::string& host, int port);
        
        bool sendPicAndOrMetaData(std::string file, std::shared_ptr<PicMetaDataObj> pPicMetaObj, const std::vector<std::string>& shareIds, const std::string& host, int port, bool both);

        void storePicDistribInfo(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::shared_ptr<PicMetaDataObj> pPicMetaObj);
        
        void sendPicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj);        

        bool sendPicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj, const std::string& host, int port);        

        void processPicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj);
        
        void checkPictureAndProcess(std::shared_ptr<PicMetaDataObj> pPicMetaObj);
        void getRemoteShareIds(std::shared_ptr<PicMetaDataObj> pPicMetaObj, 
                                std::vector<std::string>& remoteShareIds);

    
        void waitAndCopyItems();

        void processShareItems();

        void processPictures();

        void processPicMetaDatas();
        
    public:
        Distributor();
        virtual ~Distributor();
        
        std::vector<std::string> distribute(std::shared_ptr<LstObj> pLstObj);

        void distributePicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj);
        void distribute(std::shared_ptr<PicMetaDataObj> pPicMetaObj);

        void setTrnsl(MessageTranslator *pTrnsl);

        static void *entry(void *context);

        void *main();
};

#endif
