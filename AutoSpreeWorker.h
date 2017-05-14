#ifndef _AUTOSPREE_WORKER_H_
#define _AUTOSPREE_WORKER_H_

#include <Worker.h>

class AutoSpreeWorker : public Worker
{
	
	public:
		using Worker::setMaxFd;
		AutoSpreeWorker();
		~AutoSpreeWorker();
		void setMaxFd();
		void setMsgProcessors(std::shared_ptr<ApplePush> pAppleNtfy, std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify);
};
#endif
