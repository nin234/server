#include <AutoSpreeMgr.h>
#include <AutoSpreeWorker.h>
#include <Config.h>
AutoSpreeMgr::AutoSpreeMgr()
{
	pAppleNotify = std::make_shared<ApplePush>("/home/ninan/certs/AutoSpree.p12", Config::Instance().sandBox(), Config::Instance().aSpreePasswd());
    pFirebaseNotify = std::make_shared<FirebaseConnHdlr> ("516330867276@gcm.googleapis.com", "AAAAeDe3ykw:APA91bFNNEhYWFvI05qNo2xrd2C4w_dZN6dBtrvUJHjY3qs6LLFtoridHfO5G4723SzlqF54fYfp99MbDamVk80g7J19vN9rCzVsTiE3IbU4vszKHerRItPGI8Ng1wHKgyZF3-AkGJ71");
}

AutoSpreeMgr::~AutoSpreeMgr()
{

}

void
AutoSpreeMgr::setNoOfThreads(int nThrds)
{
	WorkerMgr::setNoOfThreads(nThrds);
	return;
}

Worker*
AutoSpreeMgr::getWorker()
{
	return new AutoSpreeWorker();
}

std::shared_ptr<ApplePush>
AutoSpreeMgr::getApplePushPtr()
{
	return pAppleNotify;
}

std::shared_ptr<FirebaseConnHdlr>
AutoSpreeMgr::getFirebaseConnPtr()
{
    return pFirebaseNotify;
}
