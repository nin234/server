#ifndef _EASYGROC_MGR_H_
#define _EASYGROC_MGR_H_

#include <WorkerMgr.h>

class EasyGrocMgr : public WorkerMgr
{
    
	public:

		EasyGrocMgr();
	virtual	 ~EasyGrocMgr();
	 void setNoOfThreads(int nThrds);
	Worker *getWorker();
};
#endif
