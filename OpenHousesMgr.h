#ifndef _OPENHOUSES_MGR_H_
#define _OPENHOUSES_MGR_H_

#include <WorkerMgr.h>

class OpenHousesMgr : public WorkerMgr
{
    
	public:
		OpenHousesMgr();
		virtual ~OpenHousesMgr();
		void setNoOfThreads(int nThrds);	
		Worker* getWorker();
};
#endif
