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

        std::list <DistribItem> m_shareItems;
        std::list<std::shared_ptr<PicMetaDataObj>> m_pictures;
        std::list<std::shared_ptr<PicMetaDataObj>> m_picMetaDatas;
        
        std::list <DistribItem> m_shareItemsLcl;
        std::list<std::shared_ptr<PicMetaDataObj>> m_picturesLcl;
        std::list<std::shared_ptr<PicMetaDataObj>> m_picMetaDatasLcl;

        void populateShareIdHostMap(int appId, std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::vector<std::string>& remoteShareIds);
        void createAndSendMsgs(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::shared_ptr<LstObj> pLsObj);

        void createAndSendMsgs(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::shared_ptr<PicMetaDataObj> pPicMetaObj);

        void storePicDistribInfo(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::shared_ptr<PicMetaDataObj> pPicMetaObj);
        
        void sendPicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj);        
        void processPicMetaData(std::shared_ptr<PicMetaDataObj> pPicMetaObj);
        void waitAndCopyItems();
        void processShareItems();
        void processPictures();
        
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
