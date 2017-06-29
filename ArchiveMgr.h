#ifndef _ARCHIVE_MGR_H_
#define _ARCHIVE_MGR_H_

#include <string>
#include <map>
#include <mqueue.h>
#include <mutex>
#include <Archvr.h>
#include <ShareIdArchvr.h>
#include <FrndLstArchvr.h>
#include <CommonArchvr.h>
#include <memory>
#include <list>

class ArchiveMgr
{
	std::map<std::string, mqd_t> nameMqDesMp;
	int mq_name;
	std::mutex mq_name_mutex;
	int epfd;
	void processMsgs();
	std::map<int, std::shared_ptr<Archvr>> pArchvrs;
	bool processMsg(const char *pBuf, int len);	

	public:
		ArchiveMgr();
		~ArchiveMgr();
        	static void *entry(void *context);
        	void *main();
		static ArchiveMgr& Instance();
		std::string getNextName();
		bool registerFd(std::string mqn);
		std::shared_ptr<ShareIdArchvr> getShareIdArchvr(int shidMsgTyp); 
		std::shared_ptr<FrndLstArchvr> getFrndLstArchvr(int frndLstMsgTyp); 
		std::shared_ptr<CommonArchvr> getCommonArchvr();
};


#endif
