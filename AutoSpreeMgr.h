#ifndef _AUTOSPREE_MGR_H_
#define _AUTOSPREE_MGR_H_

#include <WorkerMgr.h>

class AutoSpreeMgr : public WorkerMgr
{
    
	public:

	AutoSpreeMgr();
	virtual ~AutoSpreeMgr();
	void setNoOfThreads(int nThrds);
	Worker* getWorker();
};

#endif
