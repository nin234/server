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


bool 
SmartMsgMsgProcessor::sendApplePush(int appId, const std::vector<std::string>& tokens, const std::string& msg, int badge)
{

    return true;
}

bool 
SmartMsgMsgProcessor::sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg)
{

    return true;
}

void 
SmartMsgMsgProcessor::getSendEvents()
{

}
