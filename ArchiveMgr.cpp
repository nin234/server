#include <ArchiveMgr.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <system_error>
#include <Constants.h>
#include <mutex>
#include <sstream>
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <ShareIdArchvr.h>
#include <FrndLstArchvr.h>
#include <EasyGrocArchvr.h>
#include <Constants.h>
#include <string.h>

ArchiveMgr::ArchiveMgr()
{

	epfd = epoll_create(100);
	if (epfd < 0)
	{
		std::cout << "epoll_create failed " << std::endl;
		throw std::system_error(errno, std::system_category());	
	}

	mq_name = 0;
	pArchvrs[ARCHIVE_SHARE_ID_MSG] = std::make_shared<ShareIdArchvr>();
	pArchvrs[ARCHIVE_SHARE_TRN_ID_MSG] = std::make_shared<ShareIdArchvr>();
	pArchvrs[ARCHIVE_FRND_LST_MSG] = std::make_shared<FrndLstArchvr>();
	pArchvrs[ARCHIVE_STORE_TEMPL_LST_MSG] = std::make_shared<EasyGrocArchvr>();
	pArchvrs[ARCHIVE_EASYGROC_DEV_TKN_MSG] = std::make_shared<EasyGrocArchvr>();
	
}

ArchiveMgr::~ArchiveMgr()
{

}

void*
ArchiveMgr::entry(void *context)
{

        return reinterpret_cast<ArchiveMgr*>(context)->main();
}

void*
ArchiveMgr::main()
{
    
    
        for (;;)
        {   
		processMsgs();
	}
}

void
ArchiveMgr::processMsgs()
{
	struct epoll_event evlist[MAX_MQ_EVENTS];
	char buf[MAX_MQ_BUF];
	std::cout << "Waiting for epoll events in epoll_wait in thread=" << typeid(*this).name()  << std::endl;	
	int ready = epoll_wait(epfd, evlist, MAX_MQ_EVENTS, -1);
	if (ready == -1)
	{
		if (errno == EINTR)
		 return ;
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
			auto s = mq_receive(evlist[j].data.fd, buf , MAX_MQ_BUF, NULL);
			if (s == -1)
			{
				std::cout << "error reading on socket " << errno << std::endl;
				if (errno == EBADF)
					close(evlist[j].data.fd);
				return ;

			}
			else
			{
				//TO DO process message
				std::cout << "Received message=" << buf << std::endl;
				processMsg(buf, s);
				return ;
			}
		}
		else if (evlist[j].events &(EPOLLHUP | EPOLLERR))
		{
			std::cout << "Closing file descriptor " << evlist[j].data.fd << std::endl;
			close(evlist[j].data.fd);
			return ;
		}
	}
	
	return ;

}

bool
ArchiveMgr::processMsg(const char *pBuf, int len)
{
	int msgTyp;
	memcpy(&msgTyp, pBuf, sizeof(int));
	auto pItr = pArchvrs.find(msgTyp);
	if (pItr != pArchvrs.end())
		return pItr->second->archiveMsg(pBuf, len);

	std::cout << "Cannot find archiver for message type " << msgTyp << std::endl;
	return false;
}


ArchiveMgr&
ArchiveMgr::Instance()
{
	static ArchiveMgr instance;
	return instance;
}

std::string
ArchiveMgr::getNextName()
{
	std::lock_guard<std::mutex> lock(mq_name_mutex);
	++mq_name;
	std::ostringstream ostr;
	ostr << mq_name;
	return ostr.str();	
	
}

bool
ArchiveMgr::registerFd(mqd_t fd)
{
	std::cout << "Received new message queue register request " << fd << std::endl;
	int ret;
	 struct epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN ;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
        if (ret)
        {
		throw std::system_error(errno, std::system_category());
                close(fd);
                return false;
        }
		
	return true;
}

std::shared_ptr<ShareIdArchvr> 
ArchiveMgr::getShareIdArchvr(int shidMsgTyp)
{
	auto pItr = pArchvrs.find(shidMsgTyp);
	if (pItr != pArchvrs.end())
		return std::dynamic_pointer_cast<ShareIdArchvr>(pItr->second);
	return std::shared_ptr<ShareIdArchvr>();	
}

std::shared_ptr<FrndLstArchvr>
ArchiveMgr::getFrndLstArchvr(int frndLstMsgTyp)
{
	auto pItr = pArchvrs.find(frndLstMsgTyp);

	if (pItr != pArchvrs.end())
		return std::dynamic_pointer_cast<FrndLstArchvr>(pItr->second);
	return std::shared_ptr<FrndLstArchvr>();	

}

std::shared_ptr<EasyGrocArchvr>
ArchiveMgr::getEasyGrocArchvr()
{

	auto pItr = pArchvrs.find(ARCHIVE_STORE_TEMPL_LST_MSG);

	if (pItr != pArchvrs.end())
		return std::dynamic_pointer_cast<EasyGrocArchvr>(pItr->second);
	return std::shared_ptr<EasyGrocArchvr>();
}


