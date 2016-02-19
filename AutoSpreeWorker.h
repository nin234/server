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
		void setMsgProcessor(std::shared_ptr<ApplePush> pAppleNtfy);
};
#endif
