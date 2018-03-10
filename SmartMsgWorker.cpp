#include <SmartMsgWorker.h>
#include <Constants.h>
#include <SmartMsgMsgProcessor.h>
#include <iostream>

SmartMsgWorker::SmartMsgWorker()
{

}

SmartMsgWorker::~SmartMsgWorker()
{

}

void
SmartMsgWorker::setMaxFd()
{
	Worker::setMaxFd(SMARTMSG_MAX_FDS);
	return;
}


void
SmartMsgWorker::setMsgProcessors(std::shared_ptr<ApplePush> pAppleNtfy, std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify)
{
	std::cout << "Creating SmartMsgMsgProcessor " << __FILE__ << ":" << __LINE__ << std::endl;
	SmartMsgMsgProcessor *pMsgPr = new SmartMsgMsgProcessor();
	std::cout << "Setting SmartMsgMsgProcessor notification mechanisms " << __FILE__ << ":" << __LINE__ << std::endl;
	pMsgPr->setAppleNotify(pAppleNtfy);
    pMsgPr->setFirebaseNotify(pFirebaseNotify);
	Worker::setMsgProcessor(pMsgPr);
	return;
}
