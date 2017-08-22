#include <ConnMgr.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>


ConnMgr::ConnMgr()
{

}

ConnMgr::~ConnMgr()
{

}

void
ConnMgr::initializeListeners()
{
	int port =16789;
	for (int i=0; i < NO_OF_APPS; ++i)
	{
		apps[i].setPort(port++);
		apps[i].bindAndListen();
	}
	return;
}

std::map<int, int>
ConnMgr::waitAndGetConnections()
{
	std::map<int, int> readyFdsMp;
	fd_set readfds;
	FD_ZERO(&readfds);
	int maxfd=1;
	for (int i=0; i < NO_OF_APPS; ++i)
	{
		FD_SET(apps[i].listenFd(), &readfds);
		if (apps[i].listenFd() > maxfd)
			maxfd = apps[i].listenFd() + 1;		
	}
	

	std::cout << "Waiting in select for new connections " << __FILE__ << " " << __LINE__ << std::endl;
	int nReady =select(maxfd, &readfds, NULL, NULL, NULL);

	std::cout << "select returns " << nReady << " file descriptors ready " << __FILE__ << " " << __LINE__ << std::endl;
 	
	if (nReady>0)
	{
		for(int i=0; i<NO_OF_APPS; ++i)
		{
			if (FD_ISSET(apps[i].listenFd(), &readfds))
			{
				std::cout << "Accepting connection for app= " << i  << __FILE__ << " " << __LINE__ << std::endl;
				//push into worker thread queue
				struct 	sockaddr addr;
				socklen_t addrlen;
				int fd = accept(apps[i].listenFd(), &addr, &addrlen);
				 int one = 1;

				 setsockopt(fd, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
				readyFdsMp[i] = fd;
			
			}
		}
	}

	return readyFdsMp;

}

