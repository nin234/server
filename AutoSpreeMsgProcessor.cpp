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
AutoSpreeMsgProcessor::processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
	std::cout << "Received unhandled message " << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return;
}

