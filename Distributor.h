#ifndef _DISTRIBUTOR_H_
#define _DISTRIBUTOR_H_

#include <vector>
#include <string>
#include <MessageObjs.h>
#include <MessageTranslator.h>

class Distributor
{

        std::shared_ptr<MessageTranslator> m_pTrnsl;
    public:
        Distributor();
        virtual ~Distributor();
        
        std::vector<std::string> distribute(LstObj *pLstObj);

        void setTrnsl(MessageTranslator *pTrnsl);
};

#endif
