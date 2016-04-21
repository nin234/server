#include <EasyGrocMsgProcessor.h>
#include <EasyGrocDecoder.h>
#include <EasyGrocTranslator.h>
#include <functional>
#include <iostream>
#include <MessageObjs.h>
#include <EasyGrocDataMgr.h>
#include <ArchiveMsgCreator.h>
#include <vector>
#include <sstream>

using namespace std::placeholders;

EasyGrocMsgProcessor::EasyGrocMsgProcessor():dataStore{EasyGrocDataMgr::Instance()}
{
	MessageProcessor::setDcdTransl(new EasyGrocDecoder(), new EasyGrocTranslator());	

}

EasyGrocMsgProcessor::~EasyGrocMsgProcessor()
{

}

void
EasyGrocMsgProcessor::processMsg(const std::unique_ptr<MsgObj>& pMsg)
{
   /*
	int msgTyp = pMsg->getMsgTyp();
	
	if (msgTyp < EASY_GROC_MSG_START || msgTyp > EASY_GROC_MSG_END)
	{
		std::cout << "Invalid message type " << msgTyp << "received in EasyGrocMsgProcessor " << std::endl;
		return;
	}
	int pindx = msgTypPrcsrs[msgTyp%NO_EASYGROC_MSGS];
	if (pindx == -1)
		return;
	auto itr = processors.begin();
	itr[pindx](pMsg);
*/	
	return;
}


