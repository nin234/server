#include <SmartMsgMgr.h>
#include <SmartMsgWorker.h>
#include <iostream>
#include <Config.h>

SmartMsgMgr::SmartMsgMgr()
{
	std::cout << "SmartMsgMgr constructor " << __FILE__ << ":" << __LINE__ << std::endl;
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/certs/SmartMsg.p12", Config::Instance().sandBox());
    pFirebaseNotify = std::make_shared<FirebaseConnHdlr> ("1093967487031@gcm.googleapis.com", "AAAA_rWLIDc:APA91bHxlY5coSJxS5U7dfq6OSMgzgg5Dc2h-GBabkX5gaYmmQ15GxDVIhW6WzPdNh-tq1S9BPXYbJ5-cBE5BbJT5JUY2NDSzI_Sts4p6V2NujHSijFox-I5r6MwfP09IS9nRsdKSyV1");
	std::cout << "Constructed SmartMsgMgr " << __FILE__ << ":" << __LINE__ << std::endl;
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

std::shared_ptr<ApplePush>
SmartMsgMgr::getApplePushPtr()
{
	return pAppleNotify;
}

std::shared_ptr<FirebaseConnHdlr>
SmartMsgMgr::getFirebaseConnPtr()
{
    return pFirebaseNotify;
}
