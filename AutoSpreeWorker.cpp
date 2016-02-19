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
AutoSpreeWorker::setMsgProcessor(std::shared_ptr<ApplePush> pAppleNtfy)
{
	AutoSpreeMsgProcessor *pMsgPr = new AutoSpreeMsgProcessor();
	pMsgPr->setAppleNotify(pAppleNtfy);
	Worker::setMsgProcessor(pMsgPr);
	return;
}

