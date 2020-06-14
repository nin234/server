#include <SSLSocket.h>
#include <iostream>
#include <Util.h>

thread_local SSL_CTX* SSLSocket::pCtx = NULL;

thread_local std::mutex SSLSocket::ctx_init;

void
SSLSocket::initializeSSL()
{
	SSL_load_error_strings();
    	SSL_library_init();
    	OpenSSL_add_all_algorithms();
}

SSLSocket::~SSLSocket()
{

}

SSLSocket::SSLSocket()
{
	if (!pCtx)
	{
		std::lock_guard<std::mutex> ctxLock(ctx_init);
		if (pCtx)
			return;
		const SSL_METHOD *method;
    		method = TLS_server_method();
    		pCtx = SSL_CTX_new(method);
		if (!pCtx)
		{
			std::cerr <<Util::now() << "Failed to initialize ssl context , exiting .... " << std::endl;
			exit(0);
		}

		SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    		if (SSL_CTX_use_certificate_file(pCtx, "/home/ninan/certs/servercert.pem", SSL_FILETYPE_PEM) <= 0) 
		{
			std::cerr  <<Util::now()<< "Failed to read ssl certficate file , exiting .... " << std::endl;
			exit(0);
    		}

		if (SSL_CTX_use_PrivateKey_file(pCtx, "/home/ninan/certs/serverkey.pem", SSL_FILETYPE_PEM) <= 0 ) 
		{
			std::cerr  <<Util::now()<< "Failed to read ssl private key file , exiting .... " << std::endl;
			exit(0);
    		}
		std::cout << Util::now() << "Initialized SSL context " << __FILE__ << " " << __LINE__ << std::endl;
	}
	m_ssl = SSL_new(pCtx);
}

bool
SSLSocket::setFd(int fd)
{
	SSL_set_fd(m_ssl, fd);
	if (SSL_accept(m_ssl) <=0)
	{
		std::cerr << Util::now() << "Failed to accept SSL " << __FILE__ << " " << __LINE__ << std::endl;
		return false;
	}
	return true;
}

bool
SSLSocket::write(char *buf, int mlen)
{
	int ret = SSL_write(m_ssl, buf, mlen);
	if (ret <= 0)
	{
		std::cout << Util::now() << "Failed to send SSL message error code=" << SSL_get_error(m_ssl, ret) << " " << __FILE__ << ":" << __LINE__ << std::endl;		
		return false;
	}
	return true;
}
