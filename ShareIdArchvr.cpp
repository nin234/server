#include <ShareIdArchvr.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <system_error>
#include <Constants.h>
#include <sstream>


ShareIdArchvr::ShareIdArchvr()
{
	shareFd = open("/home/ninan/data/shareId", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
	if (shareFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/shareId " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	int nRead = read(shareFd, &shareId, sizeof(long));
	if (nRead )
	{
		if (!nRead)
			shareId = SHARE_ID_START_VAL;
		else if (nRead == -1)
			throw std::system_error(errno, std::system_category());
		else if (nRead != sizeof(long))
			shareId = SHARE_ID_START_VAL;
	}
	shareTrnFd = open("/home/ninan/data/shareTrnId", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
	if (shareTrnFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/shareTrnId " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
}

ShareIdArchvr::~ShareIdArchvr()
{
	close(shareTrnFd);
	close (shareFd);
}

void
ShareIdArchvr::populateTrnIdShrIdMp(HashMap<long, long>& trnIdShrIdMp)
{
	shrIdTrnId shtrId;
	while (true)
	{
		int numRead = read(shareTrnFd, &shtrId, sizeof(shrIdTrnId));
		if (!numRead)
			break;
		else if (numRead == -1)
			throw std::system_error(errno, std::system_category());
		else if (numRead == sizeof(shrIdTrnId))
			trnIdShrIdMp[shtrId.trnId] = shtrId.shrId;
		else
			throw std::system_error(errno, std::system_category());
	}	
	return;
}


bool
ShareIdArchvr::archiveMsg(const char *buf, int len)
{
	int msgTyp;
	memcpy(&msgTyp, buf, sizeof(int));
	switch (msgTyp)
	{
		case ARCHIVE_SHARE_ID_MSG:
		{
			return archiveShareIdMsg(buf+sizeof(int), len-sizeof(int));
		}
		break; 

		case ARCHIVE_SHARE_TRN_ID_MSG:
		{
			return archiveTrnIdShrIdMsg(buf+sizeof(int), len-sizeof(int));
		}
		break;

		default:
		{
			std::cout << "Invalid msgTyp " << __FILE__ << " " << __LINE__ << std::endl;
			return false;
		}
		break;
	}
	return true;
}

bool
ShareIdArchvr::archiveShareIdMsg(const char *buf, int len)
{
	if (len != sizeof(long))
	{
		std::cout << "Invalid shareId archive message len !=sizeof(long) " << std::endl;
		return false;
	}

	 if (lseek(shareFd, 0 , SEEK_SET) == -1)
	 {
		std::cout << "lseek failed in shareId archive " << strerror(errno) << std::endl;
		return false;
	 }
	 if (write(shareFd, buf, sizeof(long)) == -1)
	 {
		std::cout << "write failed in shareId archive " << strerror(errno) << std::endl;
		return false;
	 }
	return true;
}

bool
ShareIdArchvr::archiveTrnIdShrIdMsg(const char *buf, int len)
{
	if (len != sizeof(shrIdTrnId))
	{
		std::cout << "Invalid shareId archive message len !=sizeof(shrIdTrnId) " << std::endl;
		return false;
	}
	if (write(shareTrnFd, buf, len) == -1)
	{
		std::cout << "write failed in trnIdShrId archive " << strerror(errno) << std::endl;
		return false;
	 }
	return true;
}

long
ShareIdArchvr::getShareId()
{
	return shareId;
}


