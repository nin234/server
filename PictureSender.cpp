#include <PictureSender.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <Util.h>
#include <Constants.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

std::ostream& 
operator << (std::ostream& os, const PicFileDetails& pfd)
{
	os << " picFd=" << pfd.picFd << " picLen=" << pfd.picLen << " totWritten=" << pfd.totWritten;
	return os;
}
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
			std::cout << "Cannot get file details for " << file << " shareId=" << picNameShId.shareId << " appId=" << picNameShId.appId << " name=" << picNameShId.lstName << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
				std::cout << "Sent picMetadataMsg for file=" << file << " " << __FILE__ << ":" << __LINE__ << std::endl;
				PicFileDetails pfd;
				pfd.picLen = picNameShId.picLen;
				pfd.totWritten = 0;
				pfd.picFd = open(file.c_str(), O_RDONLY);
				pfd.shareId = picNameShId.shareIdElem;
				pfd.frndShareId = picNameShId.shareId;
				pfd.appId = picNameShId.appId;
				pfd.picName = picNameShId.lstName;	
				if (pfd.picFd != -1)
				{
					std::cout << "Picture file opend " << picNameShId << " PicFileDetails " << pfd << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
	struct timeval tv;
	gettimeofday(&tv, NULL);
	for (;;)
	{
		if (!picFdMp.size())
		{
			break;
		}
		sendPicData();
		struct timeval now;
		gettimeofday(&now, NULL);
		//sleep(1);
		if (now.tv_sec > tv.tv_sec +10)
		{
			std::cout << "More than 6 seconds in sending picture data breaking now " << __FILE__ << ":" << __LINE__ << std::endl;
			break;
		}
	}
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
		constexpr int msgId = PIC_MSG;
		memcpy(buf+sizeof(int), &msgId, sizeof(int));		
		int numread = read(pItr->second.picFd, buf+2*sizeof(int), MAX_BUF-2*sizeof(int));
		if (!numread ||  numread == -1)
		{
			std::cout << "Finished reading file " << pItr->second << " numread=" << numread << " "  << __FILE__ << ":" << __LINE__ << std::endl;
			closeAndNotify(pItr->second.picFd, fd, pItr);
			continue;
		}
		int msglen = numread + 2*sizeof(int);
		memcpy(buf, &msglen, sizeof(int));
		if (m_pObs && m_pObs->notify(buf, msglen, pItr->first))
		{
			pItr->second.totWritten += numread;
			std::cout << "Sent file contents " << pItr->second << " numread=" << numread  << " "  << __FILE__ << ":" << __LINE__ << std::endl;
			if (pItr->second.totWritten >= 	pItr->second.picLen)
			{
				std::cout << "Finished reading file " << pItr->second <<  __FILE__ << ":" << __LINE__ << std::endl;
				m_pObs->updatePicShareInfo(pItr->second.appId, pItr->second.shareId, pItr->second.frndShareId, pItr->second.picName);
				closeAndNotify(pItr->second.picFd, fd, pItr);
				continue;
			}
		}
		else
		{
			std::cout << "Failed to send file " << pItr->second << " numread=" << numread << " "  << __FILE__ << ":" << __LINE__ << std::endl;
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
	std::cout << "Inserting into picNamesShIds size before insert=" << picNamesShIds.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;
	picNamesShIds.push_back(shidlst);
	return;
}
