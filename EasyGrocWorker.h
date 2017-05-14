#ifndef _EASYGROC_WORKER_H_
#define _EASYGROC_WORKER_H_

#include <Worker.h>

class EasyGrocWorker : public Worker
{
	
	public:
		using Worker::setMaxFd;
		EasyGrocWorker();
		~EasyGrocWorker();
		void setMaxFd();
		void setMsgProcessors(std::shared_ptr<ApplePush> pAppleNotify , std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify);
};
#endif
