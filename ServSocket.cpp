#include <ServSocket.h>
#include <system_error>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/tcp.h>

ServSocket::ServSocket()
{

}

ServSocket::~ServSocket()
{

}

void 
ServSocket::bindAndListen()
{
	memset(&hintaddr, 0, sizeof(struct addrinfo));
	hintaddr.ai_canonname = NULL;
	hintaddr.ai_addr = NULL;
	hintaddr.ai_socktype = SOCK_STREAM;
	hintaddr.ai_family = AF_INET;
	hintaddr.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

	char portstr[64];
	sprintf(portstr, "%d", port);
	int ret = getaddrinfo(NULL, portstr, &hintaddr, &servaddr);
	if (ret !=0)
	{
		std::cout << "getaddrinfo failed " << gai_strerror(ret) << std::endl;
		throw std::system_error(errno, std::system_category());
	}

	int optval = 1;
	for (saddr = servaddr; saddr != NULL; saddr = saddr->ai_next)
	{
		lfd = socket(saddr->ai_family, saddr->ai_socktype, saddr->ai_protocol);
		if (lfd == -1)
			continue;
		if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		{
			throw std::system_error(errno, std::system_category());
		}
	 	int one = 1;

		 setsockopt(lfd, SOL_TCP, TCP_NODELAY, &one, sizeof(one));	
		if (bind(lfd, saddr->ai_addr, saddr->ai_addrlen) == 0)	
			break;
		close (lfd);
	}

	if (saddr == NULL)
	{
		throw std::runtime_error("Could not bind socket to any address");
	}

	if (listen(lfd, 50) == -1)	
	{
		throw std::system_error(errno, std::system_category());
	}

	freeaddrinfo(servaddr);


}
