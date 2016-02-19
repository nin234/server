#include <AutoSpreeMgr.h>
#include <AutoSpreeWorker.h>

AutoSpreeMgr::AutoSpreeMgr()
{
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/config/AutoSpree.p12", true);
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

std::shared_ptr<ApplePush>
AutoSpreeMgr::getApplePushPtr()
{
	return pAppleNotify;
}
