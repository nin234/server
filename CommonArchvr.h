#ifndef _COMMON_ARCHVR_H_
#define _COMMON_ARCHVR_H_

#include <Archvr.h>
#include <string>
#include <map>
#include <Constants.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

struct IndxKey
{
	long shareId;
	std::string name;
	bool operator < (const IndxKey& ik) const;
	
};

class CommonArchvr : public Archvr
{

		int tmplFd;
		int lstFd;
        int templLstFd;
		int deviceFd;
		int itemFd;
        	int templItemFd;
		int picMetaFd;
		bool populateArchvItemsImpl(int& appId, long& shareId, std::string& name, std::string& tmplLst);
		bool populateItemImpl(int& appId, int fd, long& shareId, std::string& name, std::string& lst);
		bool populateshareLstImpl(int& appId, int fd, long& shareId, std::string& name, long& shareIdLst, std::map<IndxKey, long>& picMetaRecIndx);
		bool populatePicMetaLstImpl(int& appId, int fd, long& shareId, std::string& name, long& shareIdLst, int& pic_len, std::map<IndxKey, long>& recIndx);
		bool populateDeviceTknImpl(int& appId, long& shareId, std::string& devId, std::string& devTkn);
		std::map<IndxKey, long> listRecIndx;
		std::map<IndxKey, long> tmplListRecIndx;
        	std::map<IndxKey, long> tmplShrlListRecIndx;
        	std::map<IndxKey, long> picMetaRecIndx;
		char wrbuf[BUF_SIZE_32K];
		bool appendLst(const IndxKey& iky , const char *buf, int len, int fd, std::map<IndxKey, long>& recIndx);
		bool updateLst(const IndxKey& iky , const char *buf, int len, int fd, long indx, std::map<IndxKey, long>& recIndx);

	public:
		CommonArchvr();
		~CommonArchvr();
		bool archiveMsg(const char *buf, int len);
		bool archiveArchvItems(const char *buf, int len);
		bool archiveShareLst(int fd, const char *buf, int len, std::map<IndxKey, long>& recIndx);
		bool archiveBuf(int fd, const char *buf, int len);
		bool archivePicMetaData(int fd, const char *buf, int len);
		bool archiveDeviceTkn(const char *buf, int len);
		template<class Op> void populateArchvItems(Op op)
		{
			if(lseek(tmplFd, 0, SEEK_SET) == -1)
			{
				std::cout << "lseek failed in tmplLst archive " << strerror(errno) << std::endl;
				return ;
			}
			while(true)
			{
				long shareId;
				std::string name, tmplLst;
				int appId;
				if (!populateArchvItemsImpl(appId, shareId, name, tmplLst))
					break;
				op(appId, shareId, name, tmplLst);
			}
		}

		template<class Op> void populateItem(Op op)
		{
			if(lseek(itemFd, 0, SEEK_SET) == -1)
			{
				std::cout << "lseek failed in shareLst archive " << strerror(errno) << std::endl;
				return ;
			}
			while(true)
			{
				long shareId;
				std::string name, lst;
				int appId;
				if (!populateItemImpl(appId, itemFd, shareId, name, lst))
					break;
				op(appId, shareId, name, lst);
			}
		}

		template<class Op> void populateShareLst(Op op)
		{
			if(lseek(lstFd, 0, SEEK_SET) == -1)
			{
				std::cout << "lseek failed in shareLst archive " << strerror(errno) << std::endl;
				return ;
			}
			while(true)
			{
				long shareId;
				std::string name;
				int appId;
                long shareIdLst;
				if (!populateshareLstImpl(appId, lstFd, shareId, name, shareIdLst, listRecIndx))
					break;
				op(appId, shareId, name, shareIdLst);
			}
		}

    template<class Op> void populateTemplItem(Op op)
    {
        if(lseek(templItemFd, 0, SEEK_SET) == -1)
        {
            std::cout << "lseek failed in shareLst archive " << strerror(errno) << std::endl;
            return ;
        }
        while(true)
        {
            long shareId;
            std::string name, lst;
            int appId;
            
            if (!populateItemImpl(appId, templItemFd, shareId, name, lst))
                break;
            op(appId, shareId, name, lst);
        }
    }

template<class Op> void populatePicMetaLst(Op op)
{
	if(lseek(picMetaFd, 0, SEEK_SET) == -1)
	{
	    std::cout << "lseek failed in shareLst archive " << strerror(errno) << std::endl;
	    return ;
	}
	while(true)
	{
	    long shareId;
	    std::string name;
	    int appId;
	    long shareIdLst;
	    int pic_len;
	    if (!populatePicMetaLstImpl(appId, picMetaFd, shareId, name, shareIdLst, pic_len, picMetaRecIndx))
		break;
	    op(appId, shareId, name, shareIdLst, pic_len);
	}
}

    
template<class Op> void populateTemplShareLst(Op op)
{
	if(lseek(templLstFd, 0, SEEK_SET) == -1)
	{
	    std::cout << "lseek failed in shareLst archive " << strerror(errno) << std::endl;
	    return ;
	}
	while(true)
	{
	    long shareId;
	    std::string name;
	    int appId;
	    long shareIdLst;
	    if (!populateshareLstImpl(appId, templLstFd, shareId, name, shareIdLst, tmplShrlListRecIndx))
		break;
	    op(appId, shareId, name, shareIdLst);
	}
}

template<class Op> void populateDeviceTkn(Op op)
{
	if(lseek(deviceFd, 0, SEEK_SET) == -1)
	{
		std::cout << "lseek failed in easyGrocDeviceTkns archive " << strerror(errno) << std::endl;
		return ;
	}
	while(true)
	{
		long shareId;
		std::string devId, devTkn;
		int appId;
		if (!populateDeviceTknImpl(appId, shareId, devId, devTkn))
			break;
		op(appId, shareId, devId, devTkn);
	}
}
		
};

#endif
