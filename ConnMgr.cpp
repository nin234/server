#include <ConnMgr.h>

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
	

	int nReady =select(maxfd, &readfds, NULL, NULL, NULL);
	//
	if (nReady>0)
	{
		for(int i=0; i<NO_OF_APPS; ++i)
		{
			if (FD_ISSET(apps[i].listenFd(), &readfds))
			{
				//push into worker thread queue
				struct 	sockaddr addr;
				socklen_t addrlen;
				int fd = accept(apps[i].listenFd(), &addr, &addrlen);
				readyFdsMp[i] = fd;
			
			}
		}
	}

	return readyFdsMp;

}

