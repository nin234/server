#include <SmartMsgMsgProcessor.h>
#include <SmartMsgDecoder.h>
#include <SmartMsgTranslator.h>
#include <iostream>

SmartMsgMsgProcessor::SmartMsgMsgProcessor()
{
	std::cout << "Setting SmartMsgDecoder and SmartMsgTranslator " << __FILE__ << ":" << __LINE__ << std::endl;	
	MessageProcessor::setDcdTransl(new SmartMsgDecoder(), new SmartMsgTranslator());
}

SmartMsgMsgProcessor::~SmartMsgMsgProcessor()
{

}

void
SmartMsgMsgProcessor::processMsg(std::shared_ptr<MsgObj> pMsg)
{

	return;
}


