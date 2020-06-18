#include <SSLSocket.h>
#include <iostream>
#include <Util.h>
#include <string>
#include <string.h>

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

int 
SSLSocket::passwd_cb(char *buf,int size,
		     int rwflag,void *userdata)
{

  std::string password = "jrevw34tq";
  
  std::cout << "In passwd_cb for SSL private key"<< " " << __FILE__ << ":" << __LINE__ << std::endl;	
  
  int pass_len = password.size() + 1;
  if (pass_len > size) 
  {
	std::cout << "Password length too big" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
    return 0;
  }
  
  strcpy(buf,password.c_str());
  return password.size();
  
}
SSLSocket::SSLSocket()
{
	if (!pCtx)
	{
		std::lock_guard<std::mutex> ctxLock(ctx_init);
		if (pCtx)
			return;
		std::cout << Util::now() << "Initialising SSL context " << __FILE__ << " " << __LINE__ << std::endl;
		const SSL_METHOD *method;
    		method = TLS_server_method();
    		pCtx = SSL_CTX_new(method);
		if (!pCtx)
		{
			std::cout <<Util::now() << "Failed to initialize ssl context , exiting .... " << std::endl;
			exit(0);
		}

		SSL_CTX_set_ecdh_auto(ctx, 1);
	        SSL_CTX_set_default_passwd_cb(pCtx, SSLSocket::passwd_cb);
    /* Set the key and cert */
		std::cout << Util::now() << "Reading certificate file " << __FILE__ << " " << __LINE__ << std::endl;
    		if (SSL_CTX_use_certificate_file(pCtx, "/home/ninan/certs/servercert.pem", SSL_FILETYPE_PEM) <= 0) 
		{
			std::cout  <<Util::now()<< "Failed to read ssl certficate file , exiting .... " << std::endl;
			exit(0);
    		}

		std::cout << Util::now() << "Reading private key file " << __FILE__ << " " << __LINE__ << std::endl;
		if (SSL_CTX_use_PrivateKey_file(pCtx, "/home/ninan/certs/serverkey.pem", SSL_FILETYPE_PEM) <= 0 ) 
		{
			std::cout  <<Util::now()<< "Failed to read ssl private key file , exiting .... " << std::endl;
			exit(0);
    		}
		std::cout << Util::now() << "Initialized SSL context " << __FILE__ << " " << __LINE__ << std::endl;
	}
	m_ssl = SSL_new(pCtx);
}

bool
SSLSocket::setFd(int fd)
{
	std::cout << "Setting ssl fd=" << fd << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	SSL_set_fd(m_ssl, fd);
	if (SSL_accept(m_ssl) <=0)
	{
		std::cout << Util::now() << "Failed to accept SSL " << __FILE__ << " " << __LINE__ << std::endl;
		return false;
	}
	std::cout << "Done Setting ssl fd=" << fd << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return true;
}

int
SSLSocket::read(char *buf, int len, bool& readAgain)
{

	int numRead = SSL_read(m_ssl, buf, len);
	if (numRead <= 0)
	{
		std::cout << Util::now() << "Failed to send SSL message error code=" << SSL_get_error(m_ssl, numRead) << " " << __FILE__ << ":" << __LINE__ << std::endl;		
		return numRead;
	}
	readAgain = SSL_pending(m_ssl);
	return numRead;
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
