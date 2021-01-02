#ifndef _DISTRIBUTOR_H_
#define _DISTRIBUTOR_H_

#include <vector>
#include <string>
#include <MessageObjs.h>
#include <MessageTranslator.h>
#include <map>
#include <DistributedDAO.h>

class Distributor
{

        std::shared_ptr<MessageTranslator> m_pTrnsl;
        void populateShareIdHostMap(int appId, std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, std::vector<std::string>& remoteShareIds);

        void createAndSendMsgs(std::map<std::pair<std::string, int>, std::vector<std::string>>& hostPortShareIds, LstObj *pLsObj);
        DistributedDAO m_distribDAO;

    public:
        Distributor();
        virtual ~Distributor();
        
        std::vector<std::string> distribute(LstObj *pLstObj);

        void setTrnsl(MessageTranslator *pTrnsl);
};

#endif
