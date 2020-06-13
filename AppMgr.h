#ifndef _APP_MGR_H_
#define _APP_MGR_H_

#include <WorkerMgr.h>
#include <vector>
#include <map>
#include <Constants.h>

class AppMgr
{
	std::map<AppName, WorkerMgr*> wrkMgr;

	public:
		AppMgr();
		~AppMgr();
		void setNewClientConnections(const std::map<AppName, int>& appIdFds);
		void setNewSSLClientConnections(const std::map<AppName, int>& appIdFds);
};

#endif
