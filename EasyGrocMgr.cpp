#include <EasyGrocMgr.h>
#include <EasyGrocWorker.h>
#include <Config.h>

EasyGrocMgr::EasyGrocMgr()
{
	std::cout << "EasyGrocMgr constructor " << __FILE__ << ":" << __LINE__ << std::endl;
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/certs/EasyGroc.p12", Config::Instance().sandBox());
    pFirebaseNotify = std::make_shared<FirebaseConnHdlr> ("Not Defined", "Not defined");
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

