#include <EasyGrocMgr.h>
#include <EasyGrocWorker.h>

EasyGrocMgr::EasyGrocMgr()
{
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/config/EasyGroc.p12", true);
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

