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
	
void 
ConnMgr::populateConnMp(std::map<AppName, int>& readyFdsMp, int i, int fd)
{
	switch(i)
	{
		case 0:
			readyFdsMp[AppName::OPENHOUSES] = fd;
		break;
		
		case 1:
			readyFdsMp[AppName::AUTOSPREE] = fd;
		break;

		case 2:
			readyFdsMp[AppName::EASYGROCLIST] = fd;
		break;
		
		case 3:
			readyFdsMp[AppName::SMARTMSG] = fd;
		break;

		default:
			std::cout << "Invalid app id i=" << i << std::endl;
		break;
	}
	return;
}

std::map<AppName, int>
ConnMgr::waitAndGetConnections()
{
	std::map<AppName, int> readyFdsMp;
	fd_set readfds;
	FD_ZERO(&readfds);
	int maxfd=1;
	for (int i=0; i < NO_OF_APPS; ++i)
	{
		FD_SET(apps[i].listenFd(), &readfds);
		//std::cout << "Setting listenFd in readfds of select listenFd=" << apps[i].listenFd() << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		if (apps[i].listenFd() +1 > maxfd)
			maxfd = apps[i].listenFd() + 1;		
	}
	

	std::cout << "Waiting in select for new connections maxfd=" << maxfd << " "<< __FILE__ << " " << __LINE__ << std::endl;
	int nReady =select(maxfd, &readfds, NULL, NULL, NULL);

	std::cout << "select returns " << nReady << " file descriptors ready " << __FILE__ << " " << __LINE__ << std::endl;
 	
	if (nReady>0)
	{
		for(int i=0; i<NO_OF_APPS; ++i)
		{
			if (FD_ISSET(apps[i].listenFd(), &readfds))
			{
				std::cout << "Accepting connection for app=" << i << " "  << __FILE__ << " " << __LINE__ << std::endl;
				//push into worker thread queue
				struct 	sockaddr addr;
				socklen_t addrlen;
				int fd = accept(apps[i].listenFd(), &addr, &addrlen);
				 int one = 1;

				 setsockopt(fd, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
				populateConnMp(readyFdsMp, i, fd);
			
			}
		}
	}

	return readyFdsMp;

}

