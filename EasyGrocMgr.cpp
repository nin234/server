#include <EasyGrocMgr.h>
#include <EasyGrocWorker.h>
#include <Config.h>

EasyGrocMgr::EasyGrocMgr()
{
	std::cout << "EasyGrocMgr constructor " << __FILE__ << ":" << __LINE__ << std::endl;
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/certs/EasyGrocList.p12", Config::Instance().sandBox(), Config::Instance().eGrocPasswd());
    pFirebaseNotify = std::make_shared<FirebaseConnHdlr> ("977230809988@gcm.googleapis.com", "AAAA44d-y4Q:APA91bHzCm1pxTgFjsQValZwOKmNejF4xd5ImJrEsSPe5VTgAFXN5BBPXG9BCS6Tj2zVnJtotfycf2vrM1n7GRd3-AwACd7-A1HkxOTjGhxHEKnN7w2zzgCcewJQylHqL3GGJElYAjoa");
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

std::shared_ptr<ApplePush>
EasyGrocMgr::getApplePushPtr()
{
	return pAppleNotify;
}

std::shared_ptr<FirebaseConnHdlr>
EasyGrocMgr::getFirebaseConnPtr()
{
    return pFirebaseNotify;
}

