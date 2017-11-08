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
	struct timeval now;
	gettimeofday(&now, NULL);
	lastCheckTime = now.tv_sec;
}

template<typename Decoder>
NtwIntf<Decoder>::~NtwIntf()
{

}

template<typename Decoder>
void
NtwIntf<Decoder>::updateFdLastActiveMp(int fd)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	fdLastActiveMp[fd] = now.tv_sec;	
	return;
}

template<typename Decoder>
void
NtwIntf<Decoder>::checkAndCleanUpIdleFds()
{
	addFdsQtoLastActiveMp();
	struct timeval now;
	gettimeofday(&now, NULL);
	if (now.tv_sec < lastCheckTime + CHECK_INTERVAL)
		return;
	for (auto pItr = fdLastActiveMp.begin(); pItr != fdLastActiveMp.end();)
	{
		if (now.tv_sec > pItr->second + CHECK_INTERVAL)
		{
			struct epoll_event event;
			event.data.fd = pItr->first;
			event.events = EPOLLIN ;
			int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, pItr->first, &event);
			if (ret)
			{
				std::cout << "epoll_ctl EPOLL_CTL_DEL failed with error=" << errno << " " << __FILE__ << ":" << __LINE__ << std::endl;	
			}
			std::cout << "Closing idle file descriptor "  << " " << __FILE__ << ":" << __LINE__ << std::endl;		
			m_pObs->onCloseFd(pItr->first);
			close(pItr->first);
			fdLastActiveMp.erase(pItr++);

		}
		else
		{
			++pItr;
		}
	}		
		
}

template<typename Decoder>
void
NtwIntf<Decoder>::closeAndCleanUpFd(int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN ;
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
	if (ret)
	{
		std::cout << "epoll_ctl EPOLL_CTL_DEL failed with error=" << errno << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	}
	m_pObs->onCloseFd(fd);
	close(fd);
	fdLastActiveMp.erase(fd);
}

template<typename Decoder> 
bool
NtwIntf<Decoder>::processMessage(char *buffer, ssize_t mlen, int fd)
{
    updateFdLastActiveMp(fd);
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
       if (pItr->second.bufIndx >= (ssize_t)sizeof(int))
       {
           int len = 0;
           memcpy(&len, pItr->second.aggrbuf, sizeof(int));
           
           ssize_t remaining = mlen;
           bMore = processFragmentedMessage(buffer, mlen, remaining, len, fd);
	  if (remaining != mlen)
		processFreshMessage(buffer+mlen-remaining, remaining, fd); 
        }
        else
        {
            int len = 0;
            ssize_t remaining = mlen;
            if (remaining +pItr->second.bufIndx < (ssize_t)sizeof(int))
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
		int origrem = remaining;
                 bMore = processFragmentedMessage(buffer, mlen, remaining, len, fd);
		if (origrem != remaining)
			processFreshMessage(buffer+mlen-remaining, remaining, fd); 
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
    auto pItr = pAggrbufs.find(fd);
    if (pItr != pAggrbufs.end())
	bufinit = true;
    while (next)
    {
        if (remaining < (ssize_t)sizeof(int))
        {
            if (!bufinit)
            {
                bufinit = aggrbufinit(fd);
    			 pItr = pAggrbufs.find(fd);
            }
            memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            pItr->second.bufIndx += remaining;
            bMore = true;
            break;
        }
        int len =0;
        memcpy(&len, buffer+mlen-remaining, sizeof(int));
	std::cout << "Message length=" << len << std::endl;	
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
            {
                memcpy(pItr->second.aggrbuf, buffer+mlen-remaining, 2*sizeof(int));
            	(*dcd)(buffer+mlen-remaining, remaining, fd);
                pItr->second.bufIndx = 2*sizeof(int);
                break;
            }
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
NtwIntf<Decoder>::processFragmentedMessage(char *buffer, ssize_t mlen, ssize_t& remaining, int len, int fd)
{
    	bool bMore = false;
    	auto pItr = pAggrbufs.find(fd);
        if (remaining == len-pItr->second.bufIndx)
        {
            if(bufferOverFlowCheck(remaining, fd))
	    {
		char tmpbuf[TMP_BUF];
		memcpy(tmpbuf, pItr->second.aggrbuf, pItr->second.bufIndx);
		memcpy(tmpbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            	(*dcd)(tmpbuf, pItr->second.bufIndx+remaining, fd);
	    }
	    else
	    {
            	memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            	(*dcd)(pItr->second.aggrbuf, len, fd);
	    }
	    pFreeList.push_back(pItr->second.aggrbuf);
	    pAggrbufs.erase(pItr);
        }
        else if (remaining < (len -pItr->second.bufIndx))
        {
            if(bufferOverFlowCheck(remaining, fd))
	    {
		char tmpbuf[TMP_BUF];
		memcpy(tmpbuf, pItr->second.aggrbuf, pItr->second.bufIndx);
		memcpy(tmpbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            	(*dcd)(tmpbuf, pItr->second.bufIndx+remaining, fd);
            	pItr->second.bufIndx = 2*sizeof(int);
	    }
	    else
	    {
            	memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            	pItr->second.bufIndx += remaining;
	    }
            bMore = true;
            
        }
        else
        {
            if(bufferOverFlowCheck(len-pItr->second.bufIndx, fd))
	    {
		char tmpbuf[TMP_BUF];
		memcpy(tmpbuf, pItr->second.aggrbuf, pItr->second.bufIndx);
		memcpy(tmpbuf+pItr->second.bufIndx, buffer+mlen-remaining, remaining);
            	(*dcd)(tmpbuf, pItr->second.bufIndx+remaining, fd);
	    }
	    else
	    {
            	memcpy(pItr->second.aggrbuf+pItr->second.bufIndx, buffer+mlen-remaining, len-pItr->second.bufIndx);
            	(*dcd)(pItr->second.aggrbuf, len, fd);
	    }
            remaining -= len - pItr->second.bufIndx;
	    pFreeList.push_back(pItr->second.aggrbuf);
	    pAggrbufs.erase(pItr);
        }

    return bMore;

}

template<typename Decoder>
void
NtwIntf<Decoder>::addFdToFdsQ(int fd)
{
	std::lock_guard<std::mutex> lock(fdsQMtx);
	fdsQ.push_back(fd);
}

template<typename Decoder>
void
NtwIntf<Decoder>::addFdsQtoLastActiveMp()
{
	std::lock_guard<std::mutex> lock(fdsQMtx);
	if (fdsQ.empty())
	{
		return;	
	}
	for (int fd : fdsQ)
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		fdLastActiveMp[fd] =  now.tv_sec;
	}
	fdsQ.clear();

}
template<typename Decoder> 
bool
NtwIntf<Decoder>::addFd (int fd)
{
	std::cout << "Received new connection request " << fd << " appId=" << dcd->getAppId() << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
	addFdToFdsQ(fd);
	return true;
}

template<typename Decoder> 
bool 
NtwIntf<Decoder>::bufferOverFlowCheck(ssize_t remaining, int fd)
{
    auto pItr = pAggrbufs.find(fd);
    if (MSG_AGGR_BUF_LEN - pItr->second.bufIndx < remaining)
    {
        return true;
    }
    return false;
}

template<typename Decoder>
bool
NtwIntf<Decoder>::waitAndGetMsg()
{
	checkAndCleanUpIdleFds();
	struct epoll_event evlist[MAX_EVENTS];
	char buf[MAX_BUF];
	//std::cout << "Waiting for epoll events in epoll_wait in thread=" << typeid(*this).name()  << std::endl;	
	int ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
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
	
	//std::cout << "File descriptors ready=" << ready << " in epoll_wait " << __FILE__ << " " << __LINE__ << std::endl;
	for (auto j=0; j < ready; ++j)
	{
		if (evlist[j].events & EPOLLIN)
		{
			auto s = read(evlist[j].data.fd, buf , MAX_BUF);
			if (s == -1)
			{
				std::cout << "error reading on socket " << std::endl;
				closeAndCleanUpFd(evlist[j].data.fd);
				return false;

			}
			else if (!s)
			{
				std::cout << "remote peer closed socket=" << evlist[j].data.fd;
				closeAndCleanUpFd(evlist[j].data.fd);
				return false;

			}
			else
			{
				//TO DO process message
				std::cout << "Received message= fd=" << evlist[j].data.fd << " length=" << s << " " << __FILE__<< ":" << __LINE__ <<  std::endl;
				processMessage(buf, s, evlist[j].data.fd);
			}
		}
		else if (evlist[j].events &(EPOLLHUP | EPOLLERR))
		{
			std::cout << "Closing file descriptor " << evlist[j].data.fd << std::endl;
			closeAndCleanUpFd(evlist[j].data.fd);
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
   	updateFdLastActiveMp(fd);
	return true;
}

template<typename Decoder>
void
NtwIntf<Decoder>::attach(NtwIntfObserver *pObs)
{
	m_pObs = pObs;
}

template class NtwIntf<MessageDecoder>;
