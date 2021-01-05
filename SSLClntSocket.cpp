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


thread_local SSL_CTX* SSLClntSocket::pCtx = NULL;

thread_local std::mutex SSLClntSocket::ctx_init;

SSLClntSocket::SSLClntSocket()
{

	if (!pCtx)
	{
		std::lock_guard<std::mutex> ctxLock(ctx_init);
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


    }
}

SSLClntSocket::~SSLClntSocket()
{
    close(m_fd);
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
    return true;
}

bool
SSLClntSocket::sendMsg(char *buf, int mlen)
{

    return true;
}
