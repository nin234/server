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
#include <CommonArchvr.h>
#include <Constants.h>
#include <string.h>

ArchiveMgr::ArchiveMgr()
{
	std::cout << "Creating ArchiveMgr " << __FILE__ << ":"<< __LINE__ << std::endl;
	epfd = epoll_create(100);
	if (epfd < 0)
	{
		std::cout << "epoll_create failed " << __FILE__ << ":"<< __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());	
	}
	std::cout << "ArchiveMgr epoll file descriptor opened " << __FILE__ << ":"<< __LINE__ << std::endl;

	mq_name = 0;
	auto pSharedIdArchvr = std::make_shared<ShareIdArchvr>();
	pArchvrs[ARCHIVE_SHARE_ID_MSG] = pSharedIdArchvr;
	pArchvrs[ARCHIVE_SHARE_TRN_ID_MSG] = pSharedIdArchvr;
	pArchvrs[ARCHIVE_FRND_LST_MSG] = std::make_shared<FrndLstArchvr>();
	auto pCommonArchvr = std::make_shared<CommonArchvr>();
	pArchvrs[ARCHIVE_ARCHIVE_ITEM_MSG] = pCommonArchvr;
	pArchvrs[ARCHIVE_DEVICE_TKN_MSG] = pCommonArchvr;
    pArchvrs[ARCHIVE_SHARE_LST_MSG] = pCommonArchvr;
    pArchvrs[ARCHIVE_ITM_MSG] = pCommonArchvr;
    pArchvrs[ARCHIVE_TEMPL_ITM_MSG] = pCommonArchvr;
    pArchvrs[ARCHIVE_SHARE_TEMPL_LST_MSG] = pCommonArchvr;
    pArchvrs[ARCHIVE_PIC_METADATA_MSG] = pCommonArchvr;
	std::cout << "Created ArchiveMgr " << __FILE__ << ":"<< __LINE__ << std::endl;
    
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
				std::cout << "error reading on socket " << strerror(errno) << " j=" << j << " ready=" << ready << " " << __FILE__ << ":" << __LINE__ << std::endl;
				if (errno == EBADF)
					close(evlist[j].data.fd);
				return ;

			}
			else
			{
				//TO DO process message
				std::cout << "Received message=" << buf << " " << __FILE__ << ":" << __LINE__ <<  std::endl;
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
	std::cout << "Received message of type=" << msgTyp << " for archiving " << __FILE__ << " " << __LINE__ << std::endl;
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
	ostr <<"/mq" <<  mq_name;
	return ostr.str();	
	
}

bool
ArchiveMgr::registerFd(std::string mqn)
{
	std::cout << "Received new message queue register request " << mqn << std::endl;
	mqd_t fd = mq_open(mqn.c_str(), O_RDONLY|O_NONBLOCK);
	if (fd == -1)
	{
		std::cout << "Failed to open message queue " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
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

std::shared_ptr<CommonArchvr>
ArchiveMgr::getCommonArchvr()
{

	auto pItr = pArchvrs.find(ARCHIVE_ARCHIVE_ITEM_MSG);

	if (pItr != pArchvrs.end())
		return std::dynamic_pointer_cast<CommonArchvr>(pItr->second);
	return std::shared_ptr<CommonArchvr>();
}


