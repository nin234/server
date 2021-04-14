#include <SSLSocket.h>
#include <iostream>
#include <Util.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


thread_local SSL_CTX* SSLSocket::pCtx = NULL;


void
SSLSocket::initializeSSL()
{
	SSL_load_error_strings();
    	SSL_library_init();
    	OpenSSL_add_all_algorithms();
}

SSLSocket::~SSLSocket()
{

    if (m_ssl)
    {
        SSL_free(m_ssl);
    }
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

SSLSocket::SSLSocket() : m_ssl(NULL), m_bAccepted(false), m_bNoShutDown(false)
{
	if (!pCtx)
	{
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
    		if (SSL_CTX_use_certificate_file(pCtx, "/home/ninan/certs/server.crt", SSL_FILETYPE_PEM) <= 0) 
		{
			std::cout  <<Util::now()<< "Failed to read ssl certficate file , exiting .... " << std::endl;
			exit(0);
    		}

		std::cout << Util::now() << "Reading private key file " << __FILE__ << " " << __LINE__ << std::endl;
		if (SSL_CTX_use_PrivateKey_file(pCtx, "/home/ninan/certs/server.key", SSL_FILETYPE_PEM) <= 0 ) 
		{
			std::cout  <<Util::now()<< "Failed to read ssl private key file , exiting .... " << std::endl;
			exit(0);
    		}
		std::cout << Util::now() << "Initialized SSL context " << __FILE__ << " " << __LINE__ << std::endl;
	}
	m_ssl = SSL_new(pCtx);
	std::cout << Util::now() << "Created new SSL object " << " " << __FILE__ << ":" << __LINE__ << std::endl;	
}

bool
SSLSocket::setFd(int fd)
{
	m_fd=fd;
	std::cout <<Util::now() <<  "Setting ssl fd=" << fd << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	int flags = fcntl(m_fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	SSL_set_fd(m_ssl, m_fd);
	return accept();
}

bool
SSLSocket::shouldAccept()
{
	return !m_bAccepted;
}

bool
SSLSocket::accept()
{
	 std::cout << Util::now() << "Done setting fd now about to SSL_accept ssl fd=" << m_fd << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	if (int ret = SSL_accept(m_ssl); ret <=0)
	{
        int err = SSL_get_error(m_ssl, ret); 
        setNoShutDownFlag(err);
		if (err == SSL_ERROR_WANT_READ)
		{
			std::cout << Util::now() << "SSL accept failed for want of DAta , will try again"<< " " << __FILE__ << ":" << __LINE__ << std::endl;
			return true;	
		}
		std::cout << Util::now() << "Failed to accept SSL " 
			  << SSL_get_error(m_ssl, ret) << " " 
		          << __FILE__ << " " << __LINE__ << std::endl;
		return false;
	}
	m_bAccepted = true;
	std::cout << Util::now() << "Accepted ssl fd=" << m_fd << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return true;

}

void
SSLSocket::setNoShutDownFlag(int err)
{
    if (err == SSL_ERROR_SSL || err == SSL_ERROR_SYSCALL)
        m_bNoShutDown = true;
}

int
SSLSocket::read(char *buf, int len, bool& readAgain)
{

	int numRead = SSL_read(m_ssl, buf, len);
	if (numRead <= 0)
	{
        char lbuf[4096];
        int err = SSL_get_error(m_ssl, numRead);
        setNoShutDownFlag(err);
		if (err == SSL_ERROR_WANT_READ)
        {
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 100000;
            nanosleep(&ts, &ts);
            readAgain = true;
        }
        else
        {
            std::cout << Util::now() << "Failed to read SSL message error code=" << err
           << " " << __FILE__ << ":" << __LINE__ << std::endl;		
            lbuf[0] = '\0';
            strerror_r(errno, lbuf, 4096); 
            std::cout << Util::now() << " error message=" << lbuf  << " " << __FILE__ << ":" << __LINE__ << std::endl; 
            readAgain = false;
        }
		return numRead;
	}
    if (SSL_pending(m_ssl))
    {
	    readAgain = true;
    }
    else
    {
	    readAgain = false;
    }
	return numRead;
}
bool
SSLSocket::write(char *buf, int mlen, bool *tryAgain)
{
	int ret = SSL_write(m_ssl, buf, mlen);
	if (ret <= 0)
	{
        int sslErr = SSL_get_error(m_ssl, ret);
        setNoShutDownFlag(sslErr);
        switch(sslErr)
        {
            case SSL_ERROR_WANT_WRITE:
            {
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 100000;
                nanosleep(&ts, &ts);
                *tryAgain =true;
            }
            break;
            
            default:
		    std::cout << Util::now() << "Failed to send SSL message error code=" << sslErr << " " << __FILE__ << ":" << __LINE__ << std::endl;		
                *tryAgain = false;
            break;
        }
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
        int sslErr = SSL_get_error(m_ssl, ret); 
        setNoShutDownFlag(sslErr);
		std::cout << Util::now() << "Failed to send SSL message error code=" << sslErr << " " << __FILE__ << ":" << __LINE__ << std::endl;		
		return false;
	}
	return true;
}

bool
SSLSocket::shutdown()
{
    if (m_bNoShutDown)
    {
        std::cout << "No shut down flag set not shutting down SSL" << " " << __FILE__ << ":" << __LINE__ << std::endl;  
        return false; 
    }
    std::cout << Util::now() << "About to shut down SSL" << " " << __FILE__ << ":" << __LINE__ << std::endl;    
	int ret = SSL_shutdown(m_ssl);
	if (ret == 1 || ret == 0)
	{
		std::cout << Util::now() << "SSL shutdown successful ret=" << ret  << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		return true;
	}
	std::cout << Util::now() << "SSL shutdown failed error code=" << SSL_get_error(m_ssl, ret)<< " ret=" << ret  << " " << __FILE__ << ":" << __LINE__ << std::endl;		
	return false;

}
