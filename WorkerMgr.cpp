#include <WorkerMgr.h>
#include <system_error>
#include <iostream>

WorkerMgr::WorkerMgr()
{
	no_of_threads = 1;
	last_client_conn = 0;
}

WorkerMgr::~WorkerMgr()
{

}

void 
WorkerMgr::initialize(int nThrds)
{
	setNoOfThreads(nThrds);
	//start workers threads
	//
	std::cout << "Initializing WorkerMgr and creating thread pool of workers " << __FILE__ << ":" << __LINE__ << std::endl;
	for (int i=0; i < no_of_threads; ++i)
	{
		Worker *pWrkr = getWorker();
		pWrkr->setMsgProcessors();
		std::cout << "Initializing Worker number " << i << " " << __FILE__ << ":" << __LINE__ << std::endl;
		pWrkr->setMaxFd();
		workers.push_back(pWrkr);
		pthread_t tid;
		if (pthread_create(&tid, NULL, &Worker::entry, pWrkr) != 0)
		{
			std::cout << "Failed to create Worker thread " << __FILE__ << ":" << __LINE__ << std::endl;
			throw std::system_error(errno, std::system_category());			
		}
	}

	//1:1 binding between Worker and Thread
	//Need a thread pool mapping of app to worker(s)
	//
}

void
WorkerMgr::setNoOfThreads(int nThrds)
{
	no_of_threads = nThrds;
	return;
}

void
WorkerMgr::setNewClientConnection(int fd)
{

	workers[last_client_conn]->addFd(fd);
	++last_client_conn;
	if (last_client_conn >= no_of_threads)
		last_client_conn = 0;
	return;
}

void
WorkerMgr::setNewSSLClientConnection(int fd)
{

	workers[last_client_conn]->addSSLFd(fd);
	++last_client_conn;
	if (last_client_conn >= no_of_threads)
		last_client_conn = 0;
	return;
}
