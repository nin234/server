#include <OpenHousesWorker.h>
#include <Constants.h>
#include <OpenHousesMsgProcessor.h>

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
	OpenHousesMsgProcessor *pMsgPr = new OpenHousesMsgProcessor();
	pMsgPr->setAppleNotify(pAppleNtfy);
    pMsgPr->setFirebaseNotify(pFirebaseNotify);
	Worker::setMsgProcessor(pMsgPr);
	return;
}
