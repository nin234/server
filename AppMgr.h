#ifndef _APP_MGR_H_
#define _APP_MGR_H_

#include <WorkerMgr.h>
#include <vector>
#include <map>

class AppMgr
{
	std::vector<WorkerMgr*> wrkMgr;

	public:
		AppMgr();
		~AppMgr();
		void setNewClientConnections(const std::map<int, int>& appIdFds);
};

#endif
