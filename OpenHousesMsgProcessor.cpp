#include <OpenHousesMsgProcessor.h>
#include <OpenHousesDecoder.h>
#include <OpenHousesTranslator.h>

OpenHousesMsgProcessor::OpenHousesMsgProcessor()
{

	MessageProcessor::setDcdTransl(new OpenHousesDecoder(), new OpenHousesTranslator());
}

OpenHousesMsgProcessor::~OpenHousesMsgProcessor()
{

}

void
OpenHousesMsgProcessor::processMsg(const std::unique_ptr<MsgObj>& pMsg)
{

	return;
}


