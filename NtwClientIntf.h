#ifndef _NTWCLIENTINTF_H_
#define _NTWCLIENTINTF_H_

#include <string>

class NtwClientIntf
{

    public:
    
        NtwClientIntf();
        virtual ~NtwClientIntf();
        
        bool sendMsg(char *buf, int mlen, const std::string& host, int port);
};

#endif
