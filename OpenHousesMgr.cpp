#include <OpenHousesMgr.h>
#include <OpenHousesWorker.h>
#include <iostream>
#include <Config.h>

OpenHousesMgr::OpenHousesMgr()
{
	std::cout << "OpenHousesMgr constructor " << __FILE__ << ":" << __LINE__ << std::endl;
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/certs/OpenHouses.p12", Config::Instance().sandBox());
    pFirebaseNotify = std::make_shared<FirebaseConnHdlr> ("1093967487031@gcm.googleapis.com", "AAAA_rWLIDc:APA91bHxlY5coSJxS5U7dfq6OSMgzgg5Dc2h-GBabkX5gaYmmQ15GxDVIhW6WzPdNh-tq1S9BPXYbJ5-cBE5BbJT5JUY2NDSzI_Sts4p6V2NujHSijFox-I5r6MwfP09IS9nRsdKSyV1");
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
