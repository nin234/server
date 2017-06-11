#include <OpenHousesMsgProcessor.h>
#include <OpenHousesDecoder.h>
#include <OpenHousesTranslator.h>
#include <iostream>

OpenHousesMsgProcessor::OpenHousesMsgProcessor()
{
	std::cout << "Setting OpenHousesDecoder and OpenHousesTranslator " << __FILE__ << ":" << __LINE__ << std::endl;	
	MessageProcessor::setDcdTransl(new OpenHousesDecoder(), new OpenHousesTranslator());
}

OpenHousesMsgProcessor::~OpenHousesMsgProcessor()
{

}

void
OpenHousesMsgProcessor::processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{

	return;
}


