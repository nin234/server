#ifndef _SSL_SOCKET_H_
#define _SSL_SOCKET_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <mutex>

class SSLSocket
{
	thread_local static SSL_CTX *pCtx;
	static thread_local std::mutex ctx_init;
	int m_fd;
	SSL *m_ssl;
	public:
		SSLSocket();
		~SSLSocket();
		static void initializeSSL();
		bool setFd(int fd);
		bool write(char *buf, int mlen);
		int read(char *buf, int len, bool& readAgain);
};

#endif
