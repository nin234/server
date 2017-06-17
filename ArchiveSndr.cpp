#include <ArchiveSndr.h>
#include <system_error>
#include <iostream>
#include <string.h>
#include <ArchiveMgr.h>

ArchiveSndr::ArchiveSndr()
{
	std::cout << " Creating ArchiveSndr " << __FILE__ << ":" << __LINE__ << std::endl;
	std::string name = ArchiveMgr::Instance().getNextName();
       std::cerr << " Open message queue name=" << name << " " << __FILE__ << ":" << __LINE__ << std::endl;
	mq_attr *mqa = new mq_attr;
	mqa->mq_maxmsg = MQ_MAXMSG;
	mqa->mq_msgsize = MQ_MSGSIZE;
	sndfd = mq_open(name.c_str(), O_CREAT|O_NONBLOCK|O_RDWR, S_IRWXU|S_IRGRP|S_IROTH, NULL);
	if (sndfd == -1)
	{
		std::cerr << " Failed to open message queue name=" << name  << " " << strerror(errno) << " " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());		 }

	ArchiveMgr::Instance().registerFd(sndfd);
	std::cout << " Created ArchiveSndr " << __FILE__ << ":" << __LINE__ << std::endl;
	
}

ArchiveSndr::~ArchiveSndr()
{

}

bool
ArchiveSndr::sendMsg(const char *pMsg, size_t len, unsigned int msg_prio)
{
	if (mq_send(sndfd, pMsg, len, msg_prio))
	{
		std::cout << "Failed to send message " << errno << " " << strerror(errno) << std::endl;
		return false;
	}
	return true;
}
