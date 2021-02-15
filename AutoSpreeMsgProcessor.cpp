#include <AutoSpreeMsgProcessor.h>
#include <AutoSpreeDecoder.h>
#include <AutoSpreeTranslator.h>
#include <iostream>

AutoSpreeMsgProcessor::AutoSpreeMsgProcessor()
{
	MessageProcessor::setDcdTransl(new AutoSpreeDecoder(), new AutoSpreeTranslator());
}

AutoSpreeMsgProcessor::~AutoSpreeMsgProcessor()
{

}

void
AutoSpreeMsgProcessor::processMsg(std::shared_ptr<MsgObj> pMsg)
{
	//std::cout << "Received unhandled message " << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return;
}

