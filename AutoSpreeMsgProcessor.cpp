#include <AutoSpreeMsgProcessor.h>
#include <AutoSpreeDecoder.h>
#include <AutoSpreeTranslator.h>

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

	return;
}

