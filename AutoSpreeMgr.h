#ifndef _AUTOSPREE_MGR_H_
#define _AUTOSPREE_MGR_H_

#include <WorkerMgr.h>

class AutoSpreeMgr : public WorkerMgr
{
	std::shared_ptr<ApplePush> pAppleNotify;
    std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify;
    
	public:

	AutoSpreeMgr();
	virtual ~AutoSpreeMgr();
	void setNoOfThreads(int nThrds);
	Worker* getWorker();
	std::shared_ptr<ApplePush> getApplePushPtr();
    std::shared_ptr<FirebaseConnHdlr> getFirebaseConnPtr();
};

#endif
