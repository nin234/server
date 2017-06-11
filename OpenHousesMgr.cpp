#include <OpenHousesMgr.h>
#include <OpenHousesWorker.h>
#include <iostream>

OpenHousesMgr::OpenHousesMgr()
{
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/certs/AutoSpree.p12", true);
    pFirebaseNotify = std::make_shared<FirebaseConnHdlr> ("Not Defined", "Not defined");
	std::cout << "Constructed OpenHousesMgr " << __FILE__ << ":" << __LINE__ << std::endl;
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

std::shared_ptr<FirebaseConnHdlr>
OpenHousesMgr::getFirebaseConnPtr()
{
    return pFirebaseNotify;
}
