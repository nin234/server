#ifndef _OPENHOUSES_MGR_H_
#define _OPENHOUSES_MGR_H_

#include <WorkerMgr.h>

class OpenHousesMgr : public WorkerMgr
{
	std::shared_ptr<ApplePush> pAppleNotify;
	public:
		OpenHousesMgr();
		virtual ~OpenHousesMgr();
		void setNoOfThreads(int nThrds);	
		Worker* getWorker();
		std::shared_ptr<ApplePush> getApplePushPtr();
};
#endif
