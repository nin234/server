#ifndef _SMARTMSG_MGR_H_
#define _SMARTMSG_MGR_H_

#include <WorkerMgr.h>

class SmartMsgMgr : public WorkerMgr
{
    
	public:
		SmartMsgMgr();
		virtual ~SmartMsgMgr();
		void setNoOfThreads(int nThrds);	
		Worker* getWorker();
};
#endif
