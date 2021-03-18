#include <EasyGrocMgr.h>
#include <EasyGrocWorker.h>
#include <Config.h>

EasyGrocMgr::EasyGrocMgr()
{
	std::cout << "EasyGrocMgr constructor " << __FILE__ << ":" << __LINE__ << std::endl;
}

EasyGrocMgr::~EasyGrocMgr()
{

}

void
EasyGrocMgr::setNoOfThreads(int nThrds)
{
	WorkerMgr::setNoOfThreads(nThrds);
	return;
}

Worker*
EasyGrocMgr::getWorker()
{
	return new EasyGrocWorker();
}


