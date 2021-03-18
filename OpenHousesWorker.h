#ifndef _OPENHOUSES_WORKER_H_
#define _OPENHOUSES_WORKER_H_

#include <Worker.h>

class OpenHousesWorker : public Worker
{
	
	public:
		OpenHousesWorker();
		~OpenHousesWorker();
		void setMaxFd();
		void setMsgProcessors();
};
#endif
