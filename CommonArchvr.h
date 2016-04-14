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
		int shrLstFd;
		int deviceFd;
		bool populateArchvItemsImpl(long& shareId, std::string& name, std::string& tmplLst);
		bool populateItemImpl(int fd, long& shareId, std::string& name, std::string& lst);
		bool populateDeviceTknImpl(long& shareId, std::string& devId, std::string& devTkn);
		std::map<IndxKey, long> listRecIndx;
		std::map<IndxKey, long> tmplListRecIndx;
		std::map<IndxKey, long> shrListRecIndx;
		char wrbuf[BUF_SIZE_32K];
		bool appendLst(const IndxKey& iky , const char *buf, int len, int fd);
		bool updateLst(const IndxKey& iky , const char *buf, int len, int fd, long indx);

	public:
		CommonArchvr();
		~CommonArchvr();
		bool archiveMsg(const char *buf, int len);
		bool archiveArchvItems(const char *buf, int len);
		bool archiveShareLst(const char *buf, int len);
		bool archiveShareLstInfo(const char *buf, int len);
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
				if (!populateArchvItemsImpl(shareId, name, tmplLst))
					break;
				op(shareId, name, tmplLst);
			}
		}

		template<class Op> void populateItem(Op op)
		{
			if(lseek(lstFd, 0, SEEK_SET) == -1)
			{
				std::cout << "lseek failed in shareLst archive " << strerror(errno) << std::endl;
				return ;
			}
			while(true)
			{
				long shareId;
				std::string name, lst;
				if (!populateItemImpl(lstFd, shareId, name, lst))
					break;
				op(shareId, name, lst);
			}
		}

		template<class Op> void populateShareLst(Op op)
		{
			if(lseek(shrLstFd, 0, SEEK_SET) == -1)
			{
				std::cout << "lseek failed in shareLst archive " << strerror(errno) << std::endl;
				return ;
			}
			while(true)
			{
				long shareId;
				std::string name, lst;
				if (!populateItemImpl(shrLstFd, shareId, name, lst))
					break;
				op(shareId, name, lst);
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
				if (!populateDeviceTknImpl(shareId, devId, devTkn))
					break;
				op(shareId, devId, devTkn);
			}
		}
		
};

#endif
