#include <EasyGrocMsgProcessor.h>
#include <EasyGrocDecoder.h>
#include <EasyGrocTranslator.h>
#include <functional>
#include <iostream>
#include <MessageObjs.h>
#include <ArchiveMsgCreator.h>
#include <vector>
#include <sstream>

using namespace std::placeholders;

EasyGrocMsgProcessor::EasyGrocMsgProcessor()
{
	MessageProcessor::setDcdTransl(new EasyGrocDecoder(), new EasyGrocTranslator());
    for (auto &msgTypPrc : msgTypPrcsrs)
        msgTypPrc = -1;
    
    msgTypPrcsrs[SHARE_TEMPL_ITEM_MSG%NO_EASYGROC_MSGS] = 0;
    
    
}

EasyGrocMsgProcessor::~EasyGrocMsgProcessor()
{

}

void
EasyGrocMsgProcessor::processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
    static auto processors = {std::bind(std::mem_fn(&EasyGrocMsgProcessor::processTemplItemMsg), this, _1)
    };

	int msgTyp = pMsg->getMsgTyp();
	
	if (msgTyp < EASY_GROC_MSG_START || msgTyp > EASY_GROC_MSG_END)
	{
		std::cout << "Invalid message type " << msgTyp << "received in EasyGrocMsgProcessor " << std::endl;
		return;
	}
    
       
    auto itr = processors.begin();
    int pindx = msgTyp%NO_EASYGROC_MSGS;
    
    if (pindx == -1)
    {
        std::cout << "No handler found for msgTyp=" << msgTyp << std::endl;
        return;
    }

    itr[msgTypPrcsrs[pindx]](pMsg);

	return;
}

void
EasyGrocMsgProcessor::processTemplItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)
{
    TemplLstObj *pTmplObj = dynamic_cast<TemplLstObj*>(pMsg.get());
    if (!pTmplObj)
    {
        std::cout << "Invalid message received in  EasyGrocMsgProcessor::processTemplItemMsg" << std::endl;
        return;
    }
    dataStore.storeTemplItem(pTmplObj->getAppId(), pTmplObj->getShrId(), pTmplObj->getName(), pTmplObj->getTemplList());
    char archbuf[32768];
    int archlen = 0;
    return;
}
