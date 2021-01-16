#ifndef _DISTRIBUTOR_H_
#define _DISTRIBUTOR_H_

#include <vector>
#include <string>
#include <MessageObjs.h>
#include <MessageTranslator.h>
#include <map>
#include <DistributedDAO.h>
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
};

class Distributor
{

        std::shared_ptr<MessageTranslator> m_pTrnsl;
        DistributedDAO m_distribDAO;
        NtwClientIntf m_ntwIntf;
        PicDistribDAO m_picDistribDAO;
        
        void populateShareIdHostMap(int appId, std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::vector<std::string>& remoteShareIds);
        void createAndSendMsgs(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, LstObj *pLsObj);
        void createAndSendMsgs(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, PicMetaDataObj *pPicMetaObj);
        void storePicDistribInfo(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, PicMetaDataObj *pPicMetaObj);
        std::list <DistribItem> m_shareItems;
        std::mutex  m_shareItemsMutex;
        std::condition_variable m_shareItemsCV;        
        std::list<std::shared_ptr<PicMetaDataObj>> m_pictures;
    public:
        Distributor();
        virtual ~Distributor();
        
        std::vector<std::string> distribute(LstObj *pLstObj);

        std::unique_ptr<PicMetaDataObj> distribute(PicMetaDataObj *pPicMetaObj);        
        void distributePicture(std::shared_ptr<PicMetaDataObj> pPicMetaObj);

        void setTrnsl(MessageTranslator *pTrnsl);

        static void *entry(void *context);

        void *main();
};

#endif
