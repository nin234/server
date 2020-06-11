#ifndef _SSL_SERV_SOCKET_H_
#define _SSL_SERV_SOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>

class SSLServSocket
{
	int port;
	struct addrinfo hintaddr;
	struct addrinfo *servaddr, *saddr;
	int lfd;

    public:
	SSLServSocket();
	~SSLServSocket();
	void setPort(int prt) {port = prt;}
	int listenFd() {return lfd;}		
	void bindAndListen();


};
#endif
 
