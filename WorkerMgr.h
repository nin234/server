#ifndef _WORKER_MGR_H_
#define _WORKER_MGR_H_

#include <vector>
#include <Worker.h>
#include <memory>
#include <ApplePush.h>
#include <FirebaseConnHdlr.h>

class WorkerMgr
{
	std::vector<Worker*> workers;
	//need to maintain various maps
	//No of fds per worker
	//app id to thread id to distribute incoming connection
	//Way to instantiate app specific class

	int no_of_threads;	
	int last_client_conn;
	public:
		WorkerMgr();
		void initialize(int nThrds);
		virtual void setNoOfThreads(int nThrds);
		virtual ~WorkerMgr();
		void setNewSSLClientConnection(int fd);
		void setNewClientConnection(int fd);
		virtual Worker* getWorker() =0;
};

#endif
