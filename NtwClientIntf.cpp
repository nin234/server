#include <NtwClientIntf.h>


NtwClientIntf::NtwClientIntf()
{

}

NtwClientIntf::~NtwClientIntf()
{

}

bool
NtwClientIntf::sendMsg(const char *buf, int mlen, const std::string& host, int port)
{
    auto pItr = m_clntConns.find(host);
    bool tryAgain = false;
    if (pItr != m_clntConns.end())
    {
        pItr->second->sendMsg(buf, mlen, host, port, &tryAgain);
    }
    else
    {
        m_clntConns[host] = std::make_unique<SSLClntSocket>();        
        m_clntConns[host]->sendMsg(buf, mlen, host , port, &tryAgain);
    }
 
    return true;
}
