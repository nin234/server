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
EasyGrocWorker::setMsgProcessor(std::shared_ptr<ApplePush> pAppleNotify)
{
	EasyGrocMsgProcessor *pMsgPr = new EasyGrocMsgProcessor();
	pMsgPr->setAppleNotify(pAppleNotify);
	Worker::setMsgProcessor(pMsgPr);
	return;
}
