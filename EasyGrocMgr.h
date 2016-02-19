#ifndef _EASYGROC_MGR_H_
#define _EASYGROC_MGR_H_

#include <WorkerMgr.h>

class EasyGrocMgr : public WorkerMgr
{
	std::shared_ptr<ApplePush> pAppleNotify;
	public:

		EasyGrocMgr();
	virtual	 ~EasyGrocMgr();
	 void setNoOfThreads(int nThrds);
	Worker *getWorker();
	std::shared_ptr<ApplePush> getApplePushPtr();
};
#endif
