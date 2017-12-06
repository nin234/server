#include <ArchiveSndr.h>
#include <system_error>
#include <iostream>
#include <string.h>
#include <ArchiveMgr.h>
#include <Constants.h>
#include <Config.h>
#include <sys/time.h>
#include <sys/resource.h>

ArchiveSndr::ArchiveSndr()
{
	std::cout << " Creating ArchiveSndr " << __FILE__ << ":" << __LINE__ << std::endl;
	std::string name = ArchiveMgr::Instance().getNextName();
       std::cout << " Open message queue name=" << name << " " << __FILE__ << ":" << __LINE__ << std::endl;
	mq_attr *mqa = new mq_attr;
	mqa->mq_maxmsg = Config::Instance().getMqMaxMsg();
	mqa->mq_msgsize = Config::Instance().getMqMsgSize();
	if (Config::Instance().getMqMsgSize() > MAX_MQ_BUF)
	{
		std::cout << " config.xml msg queue size=" <<Config::Instance().getMqMsgSize() << " greater than MAX_MQ_BUF " << MAX_MQ_BUF << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		throw std::range_error("Message queue bug size invalid");
	}
	
	sndfd = mq_open(name.c_str(), O_CREAT|O_NONBLOCK|O_RDWR, S_IRWXU|S_IRGRP|S_IROTH, mqa);
	struct rlimit lm;
	if (!getrlimit(RLIMIT_MSGQUEUE, &lm))
	{
		std::cout << "No of open files " << lm.rlim_cur << " " << lm.rlim_max << " " << __FILE__ << ":" <<__LINE__ << std::endl;
	}
	if (sndfd == -1)
	{
		std::cout << " Failed to open message queue name=" << name  << " " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());		 }
	ArchiveMgr::Instance().registerFd(name);	
	std::cout << " Created ArchiveSndr " << __FILE__ << ":" << __LINE__ << std::endl;
	
}

ArchiveSndr::~ArchiveSndr()
{

}

bool
ArchiveSndr::sendMsg(const char *pMsg, size_t len, unsigned int msg_prio)
{
	if ((int)len > Config::Instance().getMqMsgSize() )
	{
		std::cout << "Archive message len=" << len << " greater than msg queue size=" << Config::Instance().getMqMsgSize() << " " << __FILE__ << ":" << __LINE__ << std::endl;	
		return false;
	}
	if (mq_send(sndfd, pMsg, len, msg_prio))
	{
		std::cout << "Failed to send message " << errno << " " << strerror(errno) << std::endl;
		return false;
	}
	std::cout << "Sending message of len=" << len << " for archiving " << __FILE__ << " " << __LINE__ << std::endl;
	return true;
}
