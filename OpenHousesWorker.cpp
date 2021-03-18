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
OpenHousesWorker::setMsgProcessors()
{
	std::cout << "Creating OpenHousesMsgProcessor " << __FILE__ << ":" << __LINE__ << std::endl;
	OpenHousesMsgProcessor *pMsgPr = new OpenHousesMsgProcessor();
	std::cout << "Setting OpenHousesMsgProcessor notification mechanisms " << __FILE__ << ":" << __LINE__ << std::endl;
	Worker::setMsgProcessor(pMsgPr);
	return;
}
