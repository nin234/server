#include <FrndLstArchvr.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <system_error>
#include <Constants.h>
#include <string.h>
#include <MessageObjs.h>
#include <memory>

FrndLstArchvr::FrndLstArchvr()
{
	frndFd = open("/home/ninan/data/frndLst", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
	if (frndFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/frndLst " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
}

FrndLstArchvr::~FrndLstArchvr()
{
	close (frndFd);
}

bool
FrndLstArchvr::archiveMsg(const char *buf, int len)
{
	if (len <= 0)
	{
		std::cout << "Invalid length " << len << " FrndLstArchvr::archiveMsg " << std::endl;
		return false;
	}
	shrdIdSize shIdSize;
	memcpy(&shIdSize, buf+sizeof(int), sizeof(shIdSize));
	std::string frndLst = buf+sizeof(shrdIdSize) + sizeof(int);
	std::cout << "Archiving friendLst shareId=" << shIdSize.shrId << " frndLst=" << frndLst << " " << __FILE__ << ":" << __LINE__ << std::endl;
	auto pItr = frndLstIndx.find(shIdSize.shrId);

	if (pItr == frndLstIndx.end())
	{
		return appendLst(shIdSize.shrId, buf+sizeof(int), len-sizeof(int));	
	}
	else
	{
		lseek(frndFd, pItr->second, SEEK_SET);
		int size;
		read(frndFd, &size, sizeof(int));
		int datalen = size - 2*sizeof(int);
		if (len <= datalen)
		{
			lseek(frndFd, sizeof(int), SEEK_CUR);	
			if (write(frndFd, buf+sizeof(int), len-sizeof(int)) == -1)
			{
				std::cout << "Write failed to frndLst archive" << strerror(errno) << std::endl;
				return false;
			}
		}
		else
		{
			int valid =0;
			if (write(frndFd, &valid, sizeof(int)) == -1)
			{
				std::cout << "Write failed to frndLst archive" << strerror(errno) << std::endl;
				return false;
			}
			return appendLst(shIdSize.shrId, buf+sizeof(int), len-sizeof(int));	
		}
	}

	return true;
}

bool
FrndLstArchvr::appendLst(long shareId, const char *buf, int len)
{
	long offset = lseek(frndFd, 0, SEEK_END);
	int size;
	if (len >= (int)(BUF_SIZE_4K-2*sizeof(int)))
	{
		size = len*2 + 2*sizeof(int);
	}
	else
		size = BUF_SIZE_4K;
	char *pBufPt;
	std::unique_ptr<char> pBuf;
	if (size > BUF_SIZE_4K)
	{
		pBuf = std::unique_ptr<char>{new char[size]};
		pBufPt = pBuf.get();
	}
	else
	{
		pBufPt = wrbuf;
	}
	memcpy (pBufPt, &size, sizeof(int));
	int valid=1;
	memcpy (pBufPt+sizeof(int), &valid, sizeof(int));
	memcpy (pBufPt +2*sizeof(int), buf, len);
	if (write(frndFd, pBufPt, size) == -1)
	{
		std::cout << "Write failed to frndLst archive" << strerror(errno) << std::endl;
		return false;
	}
	frndLstIndx[shareId] = offset;
	return true;

}

void
FrndLstArchvr::populateFrndLstShIdMp(FrndLstMgr& frndMgr)
{

	if (lseek(frndFd, 0, SEEK_SET) == -1)
	{
		std::cout << "lseek failed in frndLst archive " << strerror(errno) << std::endl;
		return;
	}

	while (true)
	{
		int size;
		int numRead = read(frndFd, &size, sizeof(int));
		if (!numRead)
			break;
		int toread = size - sizeof(int);
		char *pBufPt;
		std::unique_ptr<char> pBuf;

		if (toread <= BUF_SIZE_4K)
		{
			numRead = read(frndFd, wrbuf, toread);
			pBufPt = wrbuf;
		}
		else
		{
			pBuf = std::unique_ptr<char>{new char[size]};			
			pBufPt = pBuf.get();
		}
		int valid;
		memcpy(&valid, pBufPt, sizeof(int));
		if (!valid)
			continue;

		shrdIdSize shIdSize;
		memcpy(&shIdSize, pBufPt+sizeof(int), sizeof(shrdIdSize));
		std::string frndLst = pBufPt+sizeof(shrdIdSize)+sizeof(int);
		frndMgr.storeFrndLst(shIdSize.shrId, frndLst);
	}

	return;
}
