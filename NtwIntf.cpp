#include <NtwIntf.h>
#include <iostream>
#include <system_error>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <MessageDecoder.h>

template<typename Decoder>
NtwIntf<Decoder>::NtwIntf()
{
	epfd = epoll_create(100);
	if (epfd < 0)
	{
		std::cout << "epoll_create failed " << std::endl;
		throw std::system_error(errno, std::system_category());	
	}
}

template<typename Decoder>
NtwIntf<Decoder>::~NtwIntf()
{

}

template<typename Decoder> 
bool
NtwIntf<Decoder>::processMessage(char *buffer, ssize_t mlen, int fd)
{
    bool bMore = false;
    auto pItr = pAggrbufs.find(fd);
    bool start = false;
    if (pItr == pAggrbufs.end())
	start = true;
    if(start)
    {
        bMore = processFreshMessage(buffer, mlen, fd);
    }
    else
    {
       if (pItr->second.bufIndx >= sizeof(int))
       {
           int len = 0;
           memcpy(&len, pItr->second.aggrbuf, sizeof(int));
           
           ssize_t remaining = mlen;
           bMore = processFragmentedMessage(buffer, mlen, remaining, len, fd);
        }
        else
        {
            int len = 0;
            ssize_t remaining = mlen;
            if (remaining +pItr->second.bufIndx < sizeof(int))
            {
                bMore = true;
                memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            }
            else
            {
                int lenRmng = sizeof(int) - pItr->second.bufIndx;
                memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, lenRmng);
                remaining -= lenRmng;
                pItr->second.bufIndx += lenRmng;
                memcpy(&len, pItr->second.aggrbuf, sizeof(int));
                 bMore = processFragmentedMessage(buffer, mlen, remaining, len, fd);
            }

        }
    }
    return bMore;

}

template<typename Decoder> 
bool 
NtwIntf<Decoder>::processFreshMessage(char *buffer, ssize_t mlen, int fd)
{
    bool bMore = false;
    bool next =true;
    ssize_t remaining = mlen;
    bool bufinit = false;	
    std::unordered_map<int, buf>::iterator pItr;
    while (next)
    {
        if (remaining < sizeof(int))
        {
		if (!bufinit)
		{
	  		 bufinit = aggrbufinit(fd);
    			 pItr = pAggrbufs.find(fd);
		}
            if(bufferOverFlowCheck(remaining, fd))
                break;
            memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            pItr->second.bufIndx += remaining;
            bMore = true;
            break;
        }
        int len =0;
        memcpy(&len, buffer, sizeof(int));
        if (remaining == len)
        {
            (*dcd)(buffer+mlen-remaining, remaining, fd);
            break;
        }
        else if (remaining < len)
        {
		if (!bufinit)
		{
			 bufinit = aggrbufinit(fd);
    			 pItr = pAggrbufs.find(fd);
		}
            if(bufferOverFlowCheck(remaining, fd))
                break;
            memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            pItr->second.bufIndx += remaining;
            bMore = true;
            break;
            
        }
        else
        {
            (*dcd)(buffer+mlen-remaining, len, fd);
            remaining -= len;
        }
    }
    return bMore;
}


template<typename Decoder> 
bool 
NtwIntf<Decoder>::aggrbufinit(int fd)
{
	auto pItr = pAggrbufs.find(fd);
	if (pItr == pAggrbufs.end())
	{
		char *pAggrBuf;
		if (pFreeList.empty())
			pAggrBuf = new char[MSG_AGGR_BUF_LEN];
		else
		{
			pAggrBuf = pFreeList.front();
			pFreeList.pop_front();
		}
		buf b{pAggrBuf, 0};
		pAggrbufs[fd] = b;
	}
	return true;
}

template<typename Decoder> 
bool 
NtwIntf<Decoder>::processFragmentedMessage(char *buffer, ssize_t mlen, int remaining, int len, int fd)
{
    bool bMore = false;
    bool next =true;
    auto pItr = pAggrbufs.find(fd);
    while (next)
    {
        if (remaining == len-pItr->second.bufIndx)
        {
            if(bufferOverFlowCheck(remaining, fd))
                break;
            memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            (*dcd)(pItr->second.aggrbuf, len, fd);
		pFreeList.push_back(pItr->second.aggrbuf);
		pAggrbufs.erase(pItr);
            break;
        }
        else if (remaining < (len -pItr->second.bufIndx))
        {
            if(bufferOverFlowCheck(remaining, fd))
                break;
            memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            pItr->second.bufIndx += remaining;
            bMore = true;
            break;
            
        }
        else
        {
            if(bufferOverFlowCheck(len-pItr->second.bufIndx, fd))
                break;
            memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, len-pItr->second.bufIndx);
            (*dcd)(pItr->second.aggrbuf, len, fd);
            pItr->second.bufIndx =0;
            remaining -= len - pItr->second.bufIndx;
        }
    }

    return bMore;

}

template<typename Decoder> 
bool
NtwIntf<Decoder>::addFd (int fd)
{
	std::cout << "Received new connection request " << fd << std::endl;
	int ret;
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN ;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
	if (ret)
	{
		std::cout << "epoll_ctl failed with error=" << errno << std::endl;
		close(fd);
		return false;
	}
	return true;
}

template<typename Decoder> 
bool 
NtwIntf<Decoder>::bufferOverFlowCheck(ssize_t remaining, int fd)
{
	auto pItr = pAggrbufs.find(fd);
	if (pItr == pAggrbufs.end())
	{
		std::cout << "Bug in the code , bufferOverFlowCheck failed " << std::endl;
		return true;
	}
    if (MSG_AGGR_BUF_LEN - pItr->second.bufIndx < remaining)
    {
        std::cout << "Invalid message received remaining " << remaining << " bufIndx=" << pItr->second.bufIndx << std::endl;
	pAggrbufs.erase(pItr);
        return true;
    }
    return false;
}

template<typename Decoder>
bool
NtwIntf<Decoder>::waitAndGetMsg()
{
	struct epoll_event evlist[MAX_EVENTS];
	char buf[MAX_BUF];
	std::cout << "Waiting for epoll events in epoll_wait in thread=" << typeid(*this).name()  << std::endl;	
	int ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
	if (ready == -1)
	{
		if (errno == EINTR)
		 return true;
		else
		{
			std::cout << "epoll_wait failed throwing system error errno=" << errno << std::endl;
			throw std::system_error(errno, std::system_category());	
		}
	}

	for (auto j=0; j < ready; ++j)
	{
		if (evlist[j].events & EPOLLIN)
		{
			auto s = read(evlist[j].data.fd, buf , MAX_BUF);
			if (s == -1)
			{
				std::cout << "error reading on socket " << std::endl;
				close(evlist[j].data.fd);
				return false;

			}
			else if (!s)
			{
				std::cout << "remote peer closed socket=" << evlist[j].data.fd;
				close(evlist[j].data.fd);
				return false;

			}
			else
			{
				//TO DO process message
				std::cout << "Received message=" << buf << std::endl;
				processMessage(buf, s, evlist[j].data.fd);
				return true;
			}
		}
		else if (evlist[j].events &(EPOLLHUP | EPOLLERR))
		{
			std::cout << "Closing file descriptor " << evlist[j].data.fd << std::endl;
			close(evlist[j].data.fd);
			return false;
		}
	}
	
	return true;
}

template<typename Decoder>
void
NtwIntf<Decoder>::setDecoder(std::shared_ptr<Decoder> pDcd)
{
	dcd = pDcd;
}

template<typename Decoder>
bool
NtwIntf<Decoder>::sendMsg(char *buf, int mlen, int fd)
{
	if (write(fd, buf, mlen) != mlen)
	{
		return false;
	}
	return true;
}

template class NtwIntf<MessageDecoder>;
