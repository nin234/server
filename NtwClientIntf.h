#ifndef _NTWCLIENTINTF_H_
#define _NTWCLIENTINTF_H_

#include <string>
#include <SSLClntSocket.h>
#include <map>
#include <memory>

class NtwClientIntf
{
        std::map<std::string, std::unique_ptr<SSLClntSocket>> m_clntConns;        
    public:
    
        NtwClientIntf();
        virtual ~NtwClientIntf();
        
        bool sendMsg(const char *buf, int mlen, const std::string& host, int port);
};

#endif
