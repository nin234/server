#ifndef _SMARTMSG_MGR_H_
#define _SMARTMSG_MGR_H_

#include <WorkerMgr.h>

class SmartMsgMgr : public WorkerMgr
{
	std::shared_ptr<ApplePush> pAppleNotify;
    std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify;
    
	public:
		SmartMsgMgr();
		virtual ~SmartMsgMgr();
		void setNoOfThreads(int nThrds);	
		Worker* getWorker();
		std::shared_ptr<ApplePush> getApplePushPtr();
        std::shared_ptr<FirebaseConnHdlr> getFirebaseConnPtr();
};
#endif
