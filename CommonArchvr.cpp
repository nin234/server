#include <CommonArchvr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <system_error>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <Constants.h>
#include <MessageObjs.h>
#include <memory>


bool
IndxKey::operator < (const IndxKey& rhs) const
{
	if (shareId < rhs.shareId && name < rhs.name)
		return true;
	return false;
}

CommonArchvr::CommonArchvr()
{
	tmplFd = open("/home/ninan/data/archiveItems", O_RDWR|O_CREAT);
	if (tmplFd == -1)
		throw std::system_error(errno, std::system_category());
	lstFd = open("/home/ninan/data/shareItems", O_RDWR|O_CREAT);
	if (lstFd == -1)
		throw std::system_error(errno, std::system_category());
	deviceFd = open("/home/ninan/data/deviceTkns", O_RDWR|O_CREAT);
	if (deviceFd == -1)
		throw std::system_error(errno, std::system_category());
	shrLstFd = open("/home/ninan/data/shareLstInfo", O_RDWR|O_CREAT);
	if (shrLstFd == -1)
		throw std::system_error(errno, std::system_category());

}

CommonArchvr::~CommonArchvr()
{
	close(tmplFd);
	close(lstFd);
	close(deviceFd);

}

bool
CommonArchvr::populateDeviceTknImpl(int& appId, long& shareId, std::string& devId, std::string& devTkn)
{
	int size;
	int numread = read(deviceFd, &size, sizeof(int));
	if (!numread)
		return false;
	int toread = size- sizeof(int);
	numread = read(deviceFd, wrbuf, toread);
	if (!numread)
		return false;
	shrdIdTemplSize devLens;
	memcpy(&devLens, wrbuf, sizeof(shrdIdTemplSize));
	shareId = devLens.shrId;
	devId = wrbuf+sizeof(shrdIdTemplSize);
	devTkn = wrbuf + sizeof(shrdIdTemplSize) + devLens.name_len;
	return true;
}

bool
CommonArchvr::populateItemImpl(int& appId, int lstFd, long& shareId, std::string& name, std::string& lst)
{
	long offset = lseek(lstFd, 0, SEEK_CUR);
	while (true)
	{
		int size;
		int numread = read(lstFd, &size, sizeof(int));
		if (!numread)
			return false;
		int toread = size- sizeof(int);
		char *pBufPt;
		std::unique_ptr<char> pBuf;

		if (toread <= BUF_SIZE_32K)
		{
			pBufPt = wrbuf;
		}	
		else
		{

			pBuf = std::unique_ptr<char>{new char[size]};
			pBufPt = pBuf.get();
		}
		numread = read(lstFd, pBufPt, toread);
		if (!numread)
			return false;

		int valid;
		memcpy(&valid, pBufPt, sizeof(int));
		if (!valid)
			continue;
		shrdIdTemplSize templSize;
		memcpy(&templSize, pBufPt+sizeof(int), sizeof(shrdIdTemplSize));
		shareId = templSize.shrId;		
		name = pBufPt+sizeof(int)+sizeof(shrdIdTemplSize);		
		lst = pBufPt+sizeof(int)+sizeof(shrdIdTemplSize) + templSize.name_len;		
		IndxKey iky;
		iky.shareId = templSize.shrId;
		iky.name = name;
		listRecIndx[iky] = offset;
		offset +=size;
		break;
	}
	return true;
}

bool
CommonArchvr::populateArchvItemsImpl(int& appId, long& shareId, std::string& name, std::string& templLst)
{
	long offset = lseek(tmplFd, 0, SEEK_CUR);
	while (true)
	{
		int size;
		int numread = read(tmplFd, &size, sizeof(int));
		if (!numread)
			return false;
		int toread = size- sizeof(int);
		char *pBufPt;
		std::unique_ptr<char> pBuf;

		if (toread <= BUF_SIZE_32K)
		{
			pBufPt = wrbuf;
		}	
		else
		{

			pBuf = std::unique_ptr<char>{new char[size]};
			pBufPt = pBuf.get();
		}
		numread = read(tmplFd, pBufPt, toread);
		if (!numread)
			return false;

		int valid;
		memcpy(&valid, pBufPt, sizeof(int));
		if (!valid)
			continue;
		shrdIdTemplSize templSize;
		memcpy(&templSize, pBufPt+sizeof(int), sizeof(shrdIdTemplSize));
		shareId = templSize.shrId;		
		name = pBufPt+sizeof(int)+sizeof(shrdIdTemplSize);		
		templLst = pBufPt+sizeof(int)+sizeof(shrdIdTemplSize) + templSize.name_len;		
		IndxKey iky;
		iky.shareId = templSize.shrId;
		iky.name = name;
		tmplListRecIndx[iky] = offset;
		offset +=size;
		break;
	}
	return true;
}



bool
CommonArchvr::archiveMsg(const char *buf, int len)
{
	int msgTyp;
	memcpy (&msgTyp, buf, sizeof(int));

	switch(msgTyp)
	{

		case ARCHIVE_ARCHIVE_ITEM_MSG:
			return archiveArchvItems(buf+sizeof(int), len-sizeof(int));
		break;

		case ARCHIVE_FRND_LST_MSG:
			return archiveShareLst(buf+sizeof(int), len-sizeof(int));
		break;

		case ARCHIVE_DEVICE_TKN_MSG:
			return archiveDeviceTkn(buf+sizeof(int), len-sizeof(int));

		break;

		case ARCHIVE_SHARE_LST_MSG:
			return archiveShareLstInfo(buf+sizeof(int), len-sizeof(int));
		break;

		default:
			std::cout << "Invalid msgTyp " << msgTyp << " received in CommonArchvr::archiveMsg " << std::endl;
			return false;	
		break;
	}
	return true;
}

bool
CommonArchvr::appendLst(const IndxKey& iky , const char *buf, int len, int fd)
{
		long offset = lseek(lstFd, 0, SEEK_END);
		int size= len*2 + 2*sizeof(int);
		char *pBufPt;
		std::unique_ptr<char> pBuf;
		if (size > BUF_SIZE_32K)
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
		if (write(fd, pBufPt, size) == -1)
		{
			std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
			return false;
		}
		if (fd == lstFd)
			listRecIndx[iky] = offset;
		else if (fd == shrLstFd)
			shrListRecIndx[iky] = offset;
		else
			tmplListRecIndx[iky] = offset;

	return true;
}

bool
CommonArchvr::archiveShareLstInfo(const char *buf, int len)
{
	if (len <=0 )
	{
		std::cout << "Invalid length " << len << " CommonArchvr::archiveShareLstInfo message " << std::endl;
		return false;
	}

	shrdIdTemplSize templSize;
	memcpy(&templSize, buf, sizeof(shrdIdTemplSize));
	IndxKey iky;
	iky.shareId = templSize.shrId;
	iky.name = buf + sizeof(shrdIdTemplSize);
	auto pItr = shrListRecIndx.find(iky);
	if (pItr == shrListRecIndx.end())
	{
		return appendLst(iky, buf, len, shrLstFd);	
	}
	else
	{
		return updateLst(iky, buf, len, shrLstFd, pItr->second);	
	}
	return true;
}

bool
CommonArchvr::updateLst(const IndxKey& iky , const char *buf, int len, int lstFd, long indx)
{
	lseek(lstFd, indx, SEEK_SET);
	int size;
	read(lstFd, &size, sizeof(int));
	int datalen = size - 2*sizeof(int);
	if (len <= datalen)
	{
		lseek(lstFd, sizeof(int), SEEK_CUR);
		if (write(lstFd, buf,len) ==-1)
		{
			std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
			return false;
		}
	}
	else
	{
		int valid = 0;
		if (write(lstFd, &valid, sizeof(int)) == -1)
		{
			std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
			return false;
		}
		return appendLst(iky, buf, len, lstFd);	
	}
	return true;
}

bool
CommonArchvr::archiveShareLst(const char *buf, int len)
{
	if (len <=0 )
	{
		std::cout << "Invalid length " << len << " CommonArchvr::archiveShareLst message " << std::endl;
		return false;
	}

	shrdIdTemplSize templSize;
	memcpy(&templSize, buf, sizeof(shrdIdTemplSize));
	IndxKey iky;
	iky.shareId = templSize.shrId;
	iky.name = buf + sizeof(shrdIdTemplSize);
	auto pItr = listRecIndx.find(iky);
	if (pItr == listRecIndx.end())
	{
		return appendLst(iky, buf, len, lstFd);	
	}
	else
	{
		return updateLst(iky, buf, len, lstFd, pItr->second);	
	}
	
	return true;
}

bool
CommonArchvr::archiveDeviceTkn(const char *buf, int len)
{
	if (len <=0 )
	{
		std::cout << "Invalid length " << len << " CommonArchvr::archiveDeviceTkn message " << std::endl;
		return false;
	}
	
	if (write(deviceFd, &len, sizeof(int)) == -1)
	{
		std::cout << "write failed in deviceFd archive " << strerror(errno) << std::endl;
		return false;
	}

	if (write(deviceFd, buf, len) == -1)
	{
		std::cout << "write failed in deviceFd archive " << strerror(errno) << std::endl;
		return false;
	}
	return true;
}

bool
CommonArchvr::archiveArchvItems(const char *buf, int len)
{
	if (len <=0 )
	{
		std::cout << "Invalid length " << len << " CommonArchvr::archiveArchvItems message " << std::endl;
		return false;
	}

	shrdIdTemplSize templSize;
	memcpy(&templSize, buf, sizeof(shrdIdTemplSize));
	IndxKey iky;
	iky.shareId = templSize.shrId;
	iky.name = buf + sizeof(shrdIdTemplSize);
	auto pItr = tmplListRecIndx.find(iky);
	if (pItr == tmplListRecIndx.end())
	{
		return appendLst(iky, buf, len, tmplFd);	
	}
	else
	{
		lseek(tmplFd, pItr->second, SEEK_SET);
		int size;
		read(tmplFd, &size, sizeof(int));
		int datalen = size - 2*sizeof(int);
		if (len <= datalen)
		{
			lseek(tmplFd, sizeof(int), SEEK_CUR);
			write(tmplFd, buf,len);
		}
		else
		{
			int valid = 0;
			if (write(tmplFd, &valid, sizeof(int)) == -1)
			{
				std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
				return false;

			}
			return appendLst(iky, buf, len, tmplFd);	
		}
	}
	
	return true;
}


