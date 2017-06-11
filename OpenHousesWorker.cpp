#include <OpenHousesWorker.h>
#include <Constants.h>
#include <OpenHousesMsgProcessor.h>
#include <iostream>

OpenHousesWorker::OpenHousesWorker()
{

}

OpenHousesWorker::~OpenHousesWorker()
{

}

void
OpenHousesWorker::setMaxFd()
{
	Worker::setMaxFd(OPENHOUSES_MAX_FDS);
	return;
}


void
OpenHousesWorker::setMsgProcessors(std::shared_ptr<ApplePush> pAppleNtfy, std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify)
{
	std::cout << "Creating OpenHousesMsgProcessor " << __FILE__ << ":" << __LINE__ << std::endl;
	OpenHousesMsgProcessor *pMsgPr = new OpenHousesMsgProcessor();
	std::cout << "Setting OpenHousesMsgProcessor notification mechanisms " << __FILE__ << ":" << __LINE__ << std::endl;
	pMsgPr->setAppleNotify(pAppleNtfy);
    pMsgPr->setFirebaseNotify(pFirebaseNotify);
	Worker::setMsgProcessor(pMsgPr);
	return;
}
