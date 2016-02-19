#include <ArchiveSndr.h>
#include <system_error>
#include <iostream>
#include <string.h>
#include <ArchiveMgr.h>

ArchiveSndr::ArchiveSndr()
{
	std::string name = ArchiveMgr::Instance().getNextName();
	sndfd = mq_open(name.c_str(), O_CREAT|O_NONBLOCK|O_WRONLY);
	if (sndfd == -1)
		throw std::system_error(errno, std::system_category());		

	ArchiveMgr::Instance().registerFd(sndfd);
	
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
