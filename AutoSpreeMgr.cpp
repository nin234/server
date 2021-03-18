#include <AutoSpreeMgr.h>
#include <AutoSpreeWorker.h>
#include <Config.h>
AutoSpreeMgr::AutoSpreeMgr()
{
}

AutoSpreeMgr::~AutoSpreeMgr()
{

}

void
AutoSpreeMgr::setNoOfThreads(int nThrds)
{
	WorkerMgr::setNoOfThreads(nThrds);
	return;
}

Worker*
AutoSpreeMgr::getWorker()
{
	return new AutoSpreeWorker();
}

