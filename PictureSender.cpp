#include <PictureSender.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <Util.h>
#include <Constants.h>
#include <unistd.h>

PictureSender::PictureSender():m_pTrnsl(NULL), m_pObs(NULL)
{


}

PictureSender::~PictureSender()
{

}

void
PictureSender::setTrnsl(MessageTranslator *pTrnsl)
{
	m_pTrnsl.reset(pTrnsl);
	return;
}

void
PictureSender::attach(Observer *pObs)
{
	m_pObs = pObs;
	return;
}

void
PictureSender::sendPicMetaDat()
{
	auto pItr = picNamesShIds.begin();
	while (pItr != picNamesShIds.end())
	{
		
		shrIdLstName picNameShId = (*pItr);
		if (picFdMp.find(picNameShId.fd) != picFdMp.end())
		{
			++pItr;
			continue;
		}
		char archbuf[32768];
		int archlen =0 ;
		std::string file = Util::constructPicFile(picNameShId.shareId, picNameShId.appId, picNameShId.lstName);
		struct stat sb;
		if (stat(file.c_str(), &sb) == -1)
		{
			std::cout << "Cannot get file details for " << file << std::endl;
			pItr = picNamesShIds.erase(pItr);
			continue;
		}
		
		if ((sb.st_mode & S_IFMT) != S_IFREG || sb.st_size < picNameShId.picLen)
		{
			std::cout << "Invalid file " << sb.st_size << " picLen=" << picNameShId.picLen << std::endl;
			pItr = picNamesShIds.erase(pItr);
			continue;
		}
		
		if (m_pTrnsl->getPicMetaMsg(archbuf, &archlen, 32768, picNameShId))
		{
			if (m_pObs && m_pObs->notify(archbuf, archlen, picNameShId.fd))
			{
				PicFileDetails pfd;
				pfd.picLen = picNameShId.picLen;
				pfd.totWritten = 0;
				pfd.picFd = open(file.c_str(), O_RDONLY);
				if (pfd.picFd != -1)
				{
					picFdMp[picNameShId.fd] = pfd;
				}
			}
		}
		pItr = picNamesShIds.erase(pItr);
	}
	return;
}

void
PictureSender::sendPictures()
{
	sendPicMetaDat();	
	sendPicData();
	return;
}

void
PictureSender::closeAndNotify(int picFd, int ntwFd, std::map<int, PicFileDetails>::iterator& pItr)
{

	close(picFd);
	picFdMp.erase(pItr++);
	if (m_pObs)
		 m_pObs->picDone(ntwFd);
	return;
}

void
PictureSender::sendPicData()
{
	auto pItr = picFdMp.begin();
	while (pItr != picFdMp.end())
	{
		char buf[MAX_BUF];
		int fd = pItr->first;
		int numread = read(pItr->second.picFd, buf, MAX_BUF);
		if (numread == -1)
		{
			closeAndNotify(pItr->second.picFd, fd, pItr);
			continue;
		}
		
		if (m_pObs && m_pObs->notify(buf, numread, pItr->first))
		{
			pItr->second.totWritten += numread;
			if (pItr->second.totWritten >= 	pItr->second.picLen)
			{
				closeAndNotify(pItr->second.picFd, fd, pItr);
				continue;
			}
		}
		else
		{
			closeAndNotify(pItr->second.picFd, fd, pItr);
			continue;
		}
		++pItr;	
	}
	return;
}

bool
PictureSender::shouldEnqueMsg(int fd)
{

	return picFdMp.find(fd) != picFdMp.end();
}

void
PictureSender::insertPicNameShid(const shrIdLstName& shidlst)
{
	picNamesShIds.push_back(shidlst);
	return;
}
