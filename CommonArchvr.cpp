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
#include <Util.h>


bool
IndxKey::operator < (const IndxKey& rhs) const
{
	if (shareId != rhs.shareId) 
	{
		return shareId < rhs.shareId;
	}
	else
	{
		return name < rhs.name;
	}
	return false;
}

CommonArchvr::CommonArchvr()
{
	tmplFd = open("/home/ninan/data/archiveItems", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
	if (tmplFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/archiveItems " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	lstFd = open("/home/ninan/data/shareItems", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
	if (lstFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/shareItems " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}

	picMetaFd = open("/home/ninan/data/picMetaItems", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
	if (picMetaFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/picMetaItems " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}

	itemFd = open("/home/ninan/data/items", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRGRP|S_IROTH);
	if (itemFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/items " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	deviceFd = open("/home/ninan/data/deviceTkns", O_RDWR|O_CREAT|O_APPEND, S_IRWXU|S_IRGRP|S_IROTH);
	if (deviceFd == -1)
	{
		std::cout << "Failed to open /home/ninan/data/items " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	
    templItemFd = open("/home/ninan/data/templItems", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
    if (templItemFd == -1)
    {
	std::cout << "Failed to open /home/ninan/data/templItems " << __FILE__ << ":" << __LINE__ << std::endl;
        throw std::system_error(errno, std::system_category());
    }
    templLstFd = open("/home/ninan/data/templShareItems", O_RDWR|O_CREAT, S_IRWXU|S_IRGRP|S_IROTH);
    if (templLstFd == -1)
    {
	std::cout << "Failed to open /home/ninan/data/templShareItems " << __FILE__ << ":" << __LINE__ << std::endl;
        throw std::system_error(errno, std::system_category());
    }
}

bool 
CommonArchvr::archivePicMetaData(int fd, const char *buf, int len)
{
	if (len <=0 )
	{
		std::cout << "Invalid length " << len << " CommonArchvr::archivePicMetaData message " << std::endl;
		return false;
	}

	picShareInfo templSize;
	memcpy(&templSize, buf, sizeof(picShareInfo));
	IndxKey iky;
	iky.shareId = templSize.shrIdLst;
	
    	iky.name = Util::stripSuffix(buf + sizeof(picShareInfo), ";");
    
    std::cout << "Archiving picMetaData=" << templSize << " name=" << iky.name << " " << __FILE__ << ":" << __LINE__ << std::endl;
	auto pItr = picMetaRecIndx.find(iky);
    
	if (pItr == picMetaRecIndx.end())
	{
		return appendLst(iky, buf, len, fd, picMetaRecIndx);
	}
	else
	{
		if (templSize.del)
		{
			return updateLst(iky, buf, len, fd, pItr->second, picMetaRecIndx);
		}
	}
	
	return true;
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
	devTknArchv devLens;
	int numread = read(deviceFd, &devLens, sizeof(devTknArchv));
	if (numread == -1)
	{
		std::cout << "Failed to read from file " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	if (!numread)
	{
		std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}
	int toread = devLens.tkn_len;
	numread = read(deviceFd, wrbuf, toread);
	if (numread == -1)
	{
		std::cout << "Failed to read from file " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	
	if (!numread)
	{
		std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}
	shareId = devLens.shareId;
	devTkn = wrbuf;
    if (devLens.platform == 0)
    {
        devId = "ios";
    }
    else if (devLens.platform == 1)
    {
        devId = "android";
    }
    appId = devLens.appId;
    
	return true;
}

bool
CommonArchvr::populateItemImpl(int& appId, int fd, long& shareId, std::string& name, std::string& lst)
{
        shrdIdTemplSize templSize;
        int numread = read(fd, &templSize, sizeof(shrdIdTemplSize));
        if (numread == -1)
	{
		std::cout << "Failed to read from file " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	
	if (!numread)
	{
		std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}
	if (numread != sizeof(shrdIdTemplSize))
	{
		std::cout << "Invalid numread=" << numread << " not equal to sizeof(shrdIdTemplSize)" << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;	
	}
        int toread = templSize.name_len + templSize.list_len;
        char *pBufPt;
        std::unique_ptr<char> pBuf;
        
        if (toread <= BUF_SIZE_32K)
        {
            pBufPt = wrbuf;
        }
        else
        {
            
            pBuf = std::unique_ptr<char>{new char[toread]};
            pBufPt = pBuf.get();
        }
        numread = read(fd, pBufPt, toread);
        if (numread == -1)
	{
		std::cout << "Failed to read from file " << __FILE__ << ":" << __LINE__ << std::endl;
		throw std::system_error(errno, std::system_category());
	}
	
	if (!numread)
	{
		std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}

	if (numread != toread)
	{
		std::cout << "Invalid numread=" << numread << " toread=" << toread << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;	
	}
        shareId = templSize.shrId;
        name = pBufPt;
        lst = pBufPt+ templSize.name_len;
	appId = templSize.appId;
    	return true;
}

bool
CommonArchvr::populatePicMetaLstImpl(int& appId, int fd, long& shareId, std::string& name, long& shareIdLst, int& pic_len, std::map<IndxKey, long>& recIndx)
{
	long offset = lseek(fd, 0, SEEK_CUR);
	while (true)
	{
		int size;
		int numread = read(fd, &size, sizeof(int));
		if (numread == -1)
		{
			std::cout << "Failed to read from file " << __FILE__ << ":" << __LINE__ << std::endl;
			throw std::system_error(errno, std::system_category());
		}
		
		if (!numread)
		{
			std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
			return false;
		}

		int toread = size- sizeof(int);
		char *pBufPt;
		std::unique_ptr<char> pBuf;
		std::cout << "pic meta size=" << toread + sizeof(int) << " " << __FILE__ << ":" << __LINE__ << std::endl;	
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
		{
			std::cout << "Failed to read from file toread=" << toread << " " << __FILE__ << ":" << __LINE__ << std::endl;
			throw std::system_error(errno, std::system_category());
		}
		
		if (!numread)
		{
			std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
			return false;
		}

		int valid;
		memcpy(&valid, pBufPt, sizeof(int));
		if (!valid)
		{
			std::cout << "valid =" << valid  << " continuing" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
			continue;
		}
		picShareInfo templSize;
		memcpy(&templSize, pBufPt+sizeof(int), sizeof(picShareInfo));
		shareId = templSize.shrId;
	        appId = templSize.appId;
		pic_len = templSize.pic_len;
		name = pBufPt+sizeof(int)+sizeof(picShareInfo);
       		 shareIdLst = templSize.shrIdLst;
		IndxKey iky;
		iky.shareId = templSize.shrIdLst;
    		iky.name = Util::stripSuffix(name, ";");
        	recIndx[iky] = offset;
        	offset +=size;
		std::cout << "Populating picmeta appId=" << appId << " shareId=" << shareId << " name=" << name << " shareIdLst=" << shareIdLst << " pic_len=" << pic_len << " " << __FILE__ << ":" << __LINE__ << std::endl;	
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
		{
			std::cout << "Failed to read from file " << __FILE__ << ":" << __LINE__ << std::endl;
			throw std::system_error(errno, std::system_category());
		}
		
		if (!numread)
		{
			std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
			return false;
		}

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
		{
			std::cout << "Failed to read from file " << __FILE__ << ":" << __LINE__ << std::endl;
			throw std::system_error(errno, std::system_category());
		}
		
		if (!numread)
		{
			std::cout << "EOF for  file " << __FILE__ << ":" << __LINE__ << std::endl;
			return false;
		}

		int valid;
		memcpy(&valid, pBufPt, sizeof(int));
		if (!valid)
			continue;
		shareInfo templSize;
		memcpy(&templSize, pBufPt+sizeof(int), sizeof(shareInfo));
		shareId = templSize.shrId;
	        appId = templSize.appId;
		name = pBufPt+sizeof(int)+sizeof(shareInfo);
       		 shareIdLst = templSize.shrIdLst;
		IndxKey iky;
		iky.shareId = shareIdLst;
		iky.name = name;
        	recIndx[iky] = offset;
		std::cout << Util::now() << "added to recIndx IndxKey.shareId="
		<<iky.shareId << " iky.name=" << iky.name << " offset=" 
		<< offset << " " << __FILE__ << ":" << __LINE__ << std::endl;		
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
		{
			std::cout << "Failed to read from /home/ninan/data/archiveItems " << __FILE__ << ":" << __LINE__ << std::endl;
			throw std::system_error(errno, std::system_category());
		}
		if (!numread)
		{
			std::cout << "File read /home/ninan/data/archiveItems complete " << __FILE__ << ":" << __LINE__ << std::endl;
			return false;
		}
		
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
		{
			std::cout << "Failed to read from /home/ninan/data/archiveItems " << __FILE__ << ":" << __LINE__ << std::endl;
			throw std::system_error(errno, std::system_category());
		}

		if (!numread)
		{
			std::cout << "File read /home/ninan/data/archiveItems complete " << __FILE__ << ":" << __LINE__ << std::endl;
			return false;
		}

		int valid;
		memcpy(&valid, pBufPt, sizeof(int));
		if (!valid)
		{
		//	std::cout << "invalid continuing " << __FILE__ << ":" << __LINE__ << std::endl;
			continue;
		}
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
	std::cout << "Archiving message of type " << msgTyp << " " << __FILE__ << ":" << __LINE__ << std::endl;
	switch(msgTyp)
	{

		case ARCHIVE_ARCHIVE_ITEM_MSG:
			return archiveArchvItems(buf+sizeof(int), len-sizeof(int));
		break;

		case ARCHIVE_SHARE_LST_MSG:
			return archiveShareLst(lstFd, buf+sizeof(int), len-sizeof(int), listRecIndx);
		break;
		
		case ARCHIVE_PIC_METADATA_MSG:
			return archivePicMetaData(picMetaFd, buf+sizeof(int), len-sizeof(int));
		break;

		case ARCHIVE_ITM_MSG:
			return archiveBuf(itemFd, buf+sizeof(int), len-sizeof(int));
		break;

		case ARCHIVE_DEVICE_TKN_MSG:
			return archiveBuf(deviceFd, buf+sizeof(int), len-sizeof(int));

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
		long offset = lseek(fd, 0, SEEK_END);
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

	if (fd == itemFd)
	{
		shrdIdTemplSize templSize;
		memcpy(&templSize, buf, sizeof(shrdIdTemplSize));
		long shareId = templSize.shrId;
		std::string name = buf+sizeof(shrdIdTemplSize);
		std::string lst = buf+sizeof(shrdIdTemplSize) + templSize.name_len;
		std::cout << "Archiving item msg shareId=" << shareId << " name=" << name << " lst=" << lst  << " len="  << len << " appId=" << templSize.appId << " " << __FILE__ << ":" << __LINE__ << std::endl;
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
	lseek(fd, indx+sizeof(int), SEEK_SET);
	int valid = 0;
	if (write(fd, &valid, sizeof(int)) == -1)
	{
		std::cout << "Write failed to shareLst archive" << strerror(errno) << std::endl;
		return false;
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
	iky.shareId = templSize.shrIdLst;
    	iky.name = buf + sizeof(shareInfo);
    
	std::cout << Util::now() << "Searching recIndx IndxKey.shareId="
	<<iky.shareId << " iky.name=" << iky.name   << " size=" << recIndx.size()
	<< " " << __FILE__ << ":" << __LINE__ << std::endl;		
	auto pItr = recIndx.find(iky);
    
	if (pItr == recIndx.end())
	{
    		std::cout << "Archiving (appending) shareLst=" << templSize << " name=" << iky.name << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return appendLst(iky, buf, len, fd, recIndx);
	}
	else
	{
		if (templSize.del)
		{
    			std::cout << "Archiving (updating) shareLst=" << templSize << " name=" << iky.name << " " << __FILE__ << ":" << __LINE__ << std::endl;
			return updateLst(iky, buf, len, fd, pItr->second, recIndx);
		}
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


