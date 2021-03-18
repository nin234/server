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
SmartMsgWorker::setMsgProcessors()
{
	std::cout << "Creating SmartMsgMsgProcessor " << __FILE__ << ":" << __LINE__ << std::endl;
	SmartMsgMsgProcessor *pMsgPr = new SmartMsgMsgProcessor();
	Worker::setMsgProcessor(pMsgPr);
	return;
}
