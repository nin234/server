#include <NtwClientIntf.h>
#include <iostream>


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
    if (pItr == m_clntConns.end())
    {
        m_clntConns[host] = std::make_unique<SSLClntSocket>();        
        pItr = m_clntConns.find(host);
        if (pItr == m_clntConns.end())
        {
            std::cout << "Cannot execute this " << " " << __FILE__ << ":" << __LINE__ << std::endl; 
            return false;
        }
    }
    bool tryAgain = false;
    
    for (int i = 0; i < 100000; ++i)
    {
        if (pItr->second->sendMsg(buf, mlen, host, port, &tryAgain))
        {
            return true;
        }

        if (!tryAgain)
        {
            return false;
        }
    }

    std::cout << "Failed to send message to host=" << host << " port=" << port 
              << " SSL_ERROR_WANT_WRITE " << " " << __FILE__ << ":" << __LINE__ << std::endl;     
    return false;
}
