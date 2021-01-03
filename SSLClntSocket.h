#ifndef _SSLCLNTSOCKET_H_
#define _SSLCLNTSOCKET_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <mutex>
#include <string>

class SSLClntSocket
{

	thread_local static SSL_CTX *pCtx;
	static thread_local std::mutex ctx_init;
	int m_fd;
	SSL *m_ssl;
    std::string host;
    int port;

    public:
        
        SSLClntSocket();

        virtual ~SSLClntSocket();
};

#endif
