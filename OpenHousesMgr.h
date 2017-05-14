#ifndef _OPENHOUSES_MGR_H_
#define _OPENHOUSES_MGR_H_

#include <WorkerMgr.h>

class OpenHousesMgr : public WorkerMgr
{
	std::shared_ptr<ApplePush> pAppleNotify;
    std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify;
    
	public:
		OpenHousesMgr();
		virtual ~OpenHousesMgr();
		void setNoOfThreads(int nThrds);	
		Worker* getWorker();
		std::shared_ptr<ApplePush> getApplePushPtr();
        std::shared_ptr<FirebaseConnHdlr> getFirebaseConnPtr();
};
#endif
