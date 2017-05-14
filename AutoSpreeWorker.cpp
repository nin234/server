#include <AutoSpreeWorker.h>
#include <Constants.h>
#include <AutoSpreeMsgProcessor.h>

AutoSpreeWorker::AutoSpreeWorker()
{

}

AutoSpreeWorker::~AutoSpreeWorker()
{

}

void
AutoSpreeWorker::setMaxFd()
{
	setMaxFd(AUTOSPREE_MAX_FDS);
}

void
AutoSpreeWorker::setMsgProcessors(std::shared_ptr<ApplePush> pAppleNtfy , std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify)
{
	AutoSpreeMsgProcessor *pMsgPr = new AutoSpreeMsgProcessor();
	pMsgPr->setAppleNotify(pAppleNtfy);
    pMsgPr->setFirebaseNotify(pFirebaseNotify);
	Worker::setMsgProcessor(pMsgPr);
	return;
}

