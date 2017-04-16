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
	itemFd = open("/home/ninan/data/items", O_RDWR|O_CREAT);
	if (itemFd == -1)
		throw std::system_error(errno, std::system_category());
	deviceFd = open("/home/ninan/data/deviceTkns", O_RDWR|O_CREAT);
	if (deviceFd == -1)
		throw std::system_error(errno, std::system_category());
	
    templItemFd = open("/home/ninan/data/templItems", O_RDWR|O_CREAT);
    if (templItemFd == -1)
        throw std::system_error(errno, std::system_category());
    templLstFd = open("/home/ninan/data/templShareItems", O_RDWR|O_CREAT);
    if (shrTemplLstFd == -1)
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
	if (numread == -1)
		return false;
	int toread = size- sizeof(int);
	numread = read(deviceFd, wrbuf, toread);
	if (numread == -1)
		return false;
	shrdIdTemplSize devLens;
	memcpy(&devLens, wrbuf, sizeof(shrdIdTemplSize));
	shareId = devLens.shrId;
	devId = wrbuf+sizeof(shrdIdTemplSize);
	devTkn = wrbuf + sizeof(shrdIdTemplSize) + devLens.name_len;
	return true;
}

bool
CommonArchvr::populateItemImpl(int& appId, int fd, long& shareId, std::string& name, std::string& lst, std::map<IndxKey, long>* recIndx)
{
    long offset = lseek(fd, 0, SEEK_CUR);
    while (true)
    {
        int size;
        int numread = read(fd, &size, sizeof(int));
        if (numread == -1)
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
        numread = read(fd, pBufPt, toread);
        if (numread == -1)
            return false;
        
        
        shrdIdTemplSize templSize;
        memcpy(&templSize, pBufPt, sizeof(shrdIdTemplSize));
        shareId = templSize.shrId;
        name = pBufPt+sizeof(int)+sizeof(shrdIdTemplSize);
        lst = pBufPt+sizeof(int)+sizeof(shrdIdTemplSize) + templSize.name_len;
        break;
    }
    return true;
}


bool
CommonArchvr::populateshareLstImpl(int& appId, int fd, long& shareId, std::string& name, long& shareIdLst, std::map<IndxKey, long>& recIndx)
{
	long offset = lseek(fd, 0, SEEK_CUR);
	while (true)
	{
		int size;
		int numread = read(fd, &size, sizeof(int));
		if (numread == -1)
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
		numread = read(fd, pBufPt, toread);
		if (numread == -1)
			return false;

		int valid;
		memcpy(&valid, pBufPt, sizeof(int));
		if (!valid)
			continue;
		shareInfo templSize;
		memcpy(&templSize, pBufPt+sizeof(int), sizeof(shareInfo));
		shareId = templSize.shrId;		
		name = pBufPt+sizeof(int)+sizeof(shareInfo);
        shareIdLst = templSize.shrIdLst;
		IndxKey iky;
		iky.shareId = templSize.shrId;
        iky.name = std::to_string(templSize.shrIdLst);
		iky.name += name;
        recIndx[iky] = offset;
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
		if (numread == -1)
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
		if (numread == -1)
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

		case ARCHIVE_SHARE_LST_MSG:
			return archiveShareLst(lstFd, buf+sizeof(int), len-sizeof(int), listRecIndx);
		break;

		case ARCHIVE_ITM_MSG:
			return archiveBuf(itemFd, buf+sizeof(int), len-sizeof(int));
		break;

		case ARCHIVE_DEVICE_TKN_MSG:
			return archiveDeviceTkn(buf+sizeof(int), len-sizeof(int));

		break;
            
        case ARCHIVE_TEMPL_ITM_MSG:
                return archiveBuf(templItemFd, buf+sizeof(int), len-sizeof(int));
            break;
        
        case ARCHIVE_SHARE_TEMPL_LST_MSG:
            return archiveShareLst(templLstFd, buf+sizeof(int), len-sizeof(int), tmplShrlListRecIndx);
            break;

        default:
			std::cout << "Invalid msgTyp " << msgTyp << " received in CommonArchvr::archiveMsg " << std::endl;
			return false;	
		break;
	}
	return true;
}

bool
CommonArchvr::appendLst(const IndxKey& iky , const char *buf, int len, int fd, std::map<IndxKey, long>& recIndx)
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
        recIndx[iky] = offset;

        return true;
}

bool
CommonArchvr::archiveBuf(int fd, const char *buf, int len)
{
	if (len <=0 )
	{
		std::cout << "Invalid length " << len << " CommonArchvr::archiveBuf message " << std::endl;
		return false;
	}

	if (write(fd, buf, len) == -1)
	{
		std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
		return false;
	}
	return true;
}

bool
CommonArchvr::updateLst(const IndxKey& iky , const char *buf, int len, int fd, long indx, std::map<IndxKey, long>& recIndx)
{
	lseek(fd, indx, SEEK_SET);
	int size;
	int nbytes = read(fd, &size, sizeof(int));
	if (nbytes == -1)
	{
		std::cout << "read failed to lstFd " << std::endl;
		return false;
	}
	int datalen = size - 2*sizeof(int);
	if (len <= datalen)
	{
		lseek(fd, sizeof(int), SEEK_CUR);
		if (write(fd, buf,len) ==-1)
		{
			std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
			return false;
		}
	}
	else
	{
		int valid = 0;
		if (write(fd, &valid, sizeof(int)) == -1)
		{
			std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
			return false;
		}
		return appendLst(iky, buf, len, fd, recIndx);
	}
	return true;
}

bool
CommonArchvr::archiveShareLst(int fd, const char *buf, int len, std::map<IndxKey, long>& recIndx)
{
	if (len <=0 )
	{
		std::cout << "Invalid length " << len << " CommonArchvr::archiveShareLst message " << std::endl;
		return false;
	}

	 shareInfo templSize;
	memcpy(&templSize, buf, sizeof(shareInfo));
	IndxKey iky;
	iky.shareId = templSize.shrId;
    iky.name = std::to_string(templSize.shrIdLst);
    iky.name += buf + sizeof(shareInfo);
    
	auto pItr = recIndx.find(iky);
	if (pItr == recIndx.end())
	{
		return appendLst(iky, buf, len, fd, recIndx);
	}
	else
	{
		return updateLst(iky, buf, len, fd, pItr->second, recIndx);
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
		return appendLst(iky, buf, len, tmplFd, tmplListRecIndx);
	}
	else
	{
		lseek(tmplFd, pItr->second, SEEK_SET);
		int size;
		int nbytes = read(tmplFd, &size, sizeof(int));
		if (nbytes == -1)
		{
			std::cout << "read failed to lstFd " << std::endl;
			return false;
		}
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
			return appendLst(iky, buf, len, tmplFd, tmplListRecIndx);
		}
	}
	
	return true;
}


