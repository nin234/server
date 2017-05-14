#include <EasyGrocWorker.h>
#include <Constants.h>
#include <EasyGrocMsgProcessor.h>


EasyGrocWorker::EasyGrocWorker()
{

}

EasyGrocWorker::~EasyGrocWorker()
{

}

void
EasyGrocWorker::setMaxFd()
{
	setMaxFd(EASYGROC_MAX_FDS);
}


void
EasyGrocWorker::setMsgProcessors(std::shared_ptr<ApplePush> pAppleNotify , std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify)
{
	EasyGrocMsgProcessor *pMsgPr = new EasyGrocMsgProcessor();
	pMsgPr->setAppleNotify(pAppleNotify);
    pMsgPr->setFirebaseNotify(pFirebaseNotify);
	Worker::setMsgProcessor(pMsgPr);
	return;
}
