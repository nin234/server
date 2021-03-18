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
AutoSpreeWorker::setMsgProcessors()
{
	AutoSpreeMsgProcessor *pMsgPr = new AutoSpreeMsgProcessor();
	Worker::setMsgProcessor(pMsgPr);
	return;
}

