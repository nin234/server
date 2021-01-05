#include <NtwClientIntf.h>


NtwClientIntf::NtwClientIntf()
{

}

NtwClientIntf::~NtwClientIntf()
{

}

bool
NtwClientIntf::sendMsg(char *buf, int mlen, const std::string& host, int port)
{
    auto pItr = m_clntConns.find(host);
    if (pItr != m_clntConns.end())
    {
        pItr->second->sendMsg(buf, mlen, host, port);
    }
    else
    {
        m_clntConns[host] = std::make_unique<SSLClntSocket>();        
        m_clntConns[host]->sendMsg(buf, mlen, host , port);
    }
 
    return true;
}
