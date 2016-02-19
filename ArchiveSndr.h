#ifndef _ARCHIVE_SNDR_H_
#define _ARCHIVE_SNDR_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string>

class ArchiveSndr
{
	mqd_t sndfd;

	public:
		ArchiveSndr();
		~ArchiveSndr();
		bool sendMsg(const char *pMsg, size_t len, unsigned int msg_prio);
};

#endif 
