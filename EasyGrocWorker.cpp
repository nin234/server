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
EasyGrocWorker::setMsgProcessors()
{
	EasyGrocMsgProcessor *pMsgPr = new EasyGrocMsgProcessor();
	Worker::setMsgProcessor(pMsgPr);
	return;
}
