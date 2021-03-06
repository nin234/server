#include <SSLClntSocket.h>
#include <iostream>
#include <Util.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <string.h>


thread_local SSL_CTX* SSLClntSocket::pCtx = NULL;


SSLClntSocket::SSLClntSocket() : m_ssl(NULL), m_bConnected(false)
{

	if (!pCtx)
	{
		if (pCtx)
			return;
		std::cout << Util::now() << "Initialising SSL client context " << __FILE__ << " " << __LINE__ << std::endl;
		const SSL_METHOD *method;
		method = TLS_client_method();
		pCtx = SSL_CTX_new(method);
		if (!pCtx)
		{
			std::cout <<Util::now() << "Failed to initialize ssl client context , exiting .... " << std::endl;
			exit(0);
		}
		if (SSL_CTX_use_certificate_file(pCtx, "/home/ninan/certs/client.crt", SSL_FILETYPE_PEM) <= 0) 
		{
			std::cout  <<Util::now()<< "Failed to read ssl client certficate file , exiting .... " << " " << __FILE__ << ":" << __LINE__ << std::endl;  
			exit(0);
    	}



    }
}

SSLClntSocket::~SSLClntSocket()
{
    close(m_fd);
    if (m_ssl)
    {
        SSL_free(m_ssl);
    }
}

bool
SSLClntSocket::connect(std::string host, int port)
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);

    std::cout << Util::now() << "Connecting to server host=" << host << " port=" 
                << port << " " << __FILE__ << ":" << __LINE__ << std::endl; 
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_addr = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol   = 0;
    hints.ai_flags = AI_NUMERICSERV;
   
    std::stringstream ss;
    ss << port; 
    int ret = getaddrinfo(host.c_str(), ss.str().c_str(), &hints, &result);
    if ( ret)
    {
        std::cout << Util::now() << "getaddrinfo failed for host=" << host << " port="
                  << port << " error=" << gai_strerror(ret)
                  << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        return false;
    }
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        m_fd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (m_fd == -1)
            continue;
        if (::connect(m_fd, rp->ai_addr, rp->ai_addrlen) != -1) {

            std::cout << Util::now()  << "Connected to host=" << host << " sin_port=" 
            << ((struct sockaddr_in*)rp->ai_addr)->sin_port << " port=" 
            << port << " " << __FILE__ << ":" << __LINE__ << std::endl;  
            break;
        }
        close(m_fd);
    }

    if (rp == NULL)
    {
        std::cout << Util::now() << "Could not connect socket to any address" << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    m_ssl = SSL_new(pCtx);
    if (!m_ssl)
    {
        std::cout << Util::now() << "Error creating ssl object" << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }

    SSL_set_fd(m_ssl, m_fd);

    ret = SSL_connect(m_ssl);
    if (ret  <= 0)
    {
        std::cout << Util::now() << "Failed to connect SSL" << SSL_get_error(m_ssl, ret) << " " << __FILE__ << ":" << __LINE__ << std::endl;    
        return false;
    }
    std::cout << Util::now() << "SSL connection established to host="
               << host << " port=" << port << " " << __FILE__ << ":" << __LINE__ << std::endl;  
/*
    X509* cert = SSL_get_peer_certificate(m_ssl);
    if (cert == NULL)
    {
        std::cout << Util::now() << "Failed to get peer certificate " << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        return false;
    }

    long res = SSL_get_verify_result(m_ssl);
    if(!(X509_V_OK == res)) 
    {
        std::cout << Util::now() << "Failed to verify certificate " << " " << __FILE__ << ":" << __LINE__ << std::endl;   
        return false;
    }
*/
    return true;
}

bool
SSLClntSocket::sendMsg(const char *buf, int mlen, const std::string& host, int port, bool *tryAgain)
{
    if (!m_bConnected)
    {
        if (!connect(host, port))
        {
            std::cout << Util::now() << "Failed to send message not connected to host=" << host << " port=" << port
            << " " << __FILE__ << ":" << __LINE__ << std::endl; 
            *tryAgain = false;
            return false;
        }
        m_bConnected = true;
    }   
   
    std::cout << "Sending message of len=" << mlen << " buf=" << buf << " " << __FILE__ << ":" << __LINE__ << std::endl;     
     int ret = SSL_write(m_ssl, buf, mlen);
     if (ret <= 0)
     {
         int sslErr = SSL_get_error(m_ssl, ret);
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
                if (sslErr == 5)
                {
                    char buf[1024];
                    strerror_r(errno, buf, 1024);
                    std::cout << Util::now() << "error=" << buf << " errno=" << errno << " " << __FILE__ << ":" << __LINE__ << std::endl;  
                }
                *tryAgain = false;
            break;
        }
		return false;

     } 
    return true;
}
