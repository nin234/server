#ifndef _SSL_SOCKET_H_
#define _SSL_SOCKET_H_

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <mutex>

class SSLSocket
{
	thread_local static SSL_CTX *pCtx;
	int m_fd;
	SSL *m_ssl;
	bool m_bAccepted;
	public:
		SSLSocket();
		~SSLSocket();
		static void initializeSSL();
		bool setFd(int fd);
		bool write(char *buf, int mlen);
		int read(char *buf, int len, bool& readAgain);
		static int passwd_cb(char *buf,int size,  int rwflag,void *userdata);
		bool shutdown();
		bool accept();
		bool shouldAccept();
		bool write(char *buf, int mlen, bool* tryAgain);
};

#endif
