#include <SmartMsgMgr.h>
#include <SmartMsgWorker.h>
#include <iostream>
#include <Config.h>

SmartMsgMgr::SmartMsgMgr()
{
	std::cout << "SmartMsgMgr constructor " << __FILE__ << ":" << __LINE__ << std::endl;
}

SmartMsgMgr::~SmartMsgMgr()
{

}

void
SmartMsgMgr::setNoOfThreads(int nThrds)
{
	WorkerMgr::setNoOfThreads(nThrds);
	return;
}

Worker*
SmartMsgMgr::getWorker()
{
	return new SmartMsgWorker();
}


