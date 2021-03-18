#include <OpenHousesMgr.h>
#include <OpenHousesWorker.h>
#include <iostream>
#include <Config.h>

OpenHousesMgr::OpenHousesMgr()
{
	std::cout << "OpenHousesMgr constructor " << __FILE__ << ":" << __LINE__ << std::endl;
}

OpenHousesMgr::~OpenHousesMgr()
{

}

void
OpenHousesMgr::setNoOfThreads(int nThrds)
{
	WorkerMgr::setNoOfThreads(nThrds);
	return;
}

Worker*
OpenHousesMgr::getWorker()
{
	return new OpenHousesWorker();
}

