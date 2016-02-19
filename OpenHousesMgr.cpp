#include <OpenHousesMgr.h>
#include <OpenHousesWorker.h>

OpenHousesMgr::OpenHousesMgr()
{
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/config/OpenHouses.p12", true);
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

std::shared_ptr<ApplePush>
OpenHousesMgr::getApplePushPtr()
{
	return pAppleNotify;
}
