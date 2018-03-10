#ifndef _SMARTMSG_WORKER_H_
#define _SMARTMSG_WORKER_H_

#include <Worker.h>

class SmartMsgWorker : public Worker
{
	
	public:
		SmartMsgWorker();
		~SmartMsgWorker();
		void setMaxFd();
		void setMsgProcessors(std::shared_ptr<ApplePush> pAppleNtfy, std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify);
};
#endif
