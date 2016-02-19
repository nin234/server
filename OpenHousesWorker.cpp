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
OpenHousesWorker::setMsgProcessor(std::shared_ptr<ApplePush> pAppleNtfy)
{
	OpenHousesMsgProcessor *pMsgPr = new OpenHousesMsgProcessor();
	pMsgPr->setAppleNotify(pAppleNtfy);
	Worker::setMsgProcessor(pMsgPr);
	return;
}
