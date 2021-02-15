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
    
}

EasyGrocMsgProcessor::~EasyGrocMsgProcessor()
{

}

void
EasyGrocMsgProcessor::processMsg(std::shared_ptr<MsgObj> pMsg)
{
	int msgTyp = pMsg->getMsgTyp();
	switch(msgTyp)
	{
		case SHARE_TEMPL_ITEM_MSG:
			return processTemplItemMsg(pMsg);
		default:
//			std::cout << "Unhandled message type=" << msgTyp << " " << __FILE__ << ":" << __LINE__ << std::endl;		
		break;
	}
	return;
}

void
EasyGrocMsgProcessor::processTemplItemMsg(std::shared_ptr<MsgObj> pMsg)
{
    auto pTmplObj = std::dynamic_pointer_cast<TemplLstObj>(pMsg);
    if (!pTmplObj)
    {
        std::cout << "Invalid message received in  EasyGrocMsgProcessor::processTemplItemMsg" << std::endl;
        return;
    }
   std::cout << "Received Templ Item message appId=" << pTmplObj->getAppId() << " shareId=" << pTmplObj->getShrId() << " name=" << pTmplObj->getName() << " item=" << pTmplObj->getTemplList() << " " << __FILE__ << ":" << __LINE__ << std::endl;
	std::string tmplLstToStore;
	std::string::size_type xpos = pTmplObj->getTemplList().find(":::");
	if (xpos == std::string::npos)
	{
		std::cout << "Invalid templ item received item=" << pTmplObj->getTemplList() << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return;
	}
	tmplLstToStore = pTmplObj->getTemplList().substr(xpos+3);
    dataStore.storeTemplItem(pTmplObj->getAppId(), pTmplObj->getShrId(), pTmplObj->getName(), tmplLstToStore);
    char archbuf[32768];
    int archlen = 0;
    if (ArchiveMsgCreator::createTemplItemMsg(archbuf, archlen, pTmplObj->getAppId(), pTmplObj->getShrId(), pTmplObj->getName(), pTmplObj->getTemplList(), 32768))
        sendArchiveMsg(archbuf , archlen, 10);
    std::vector<std::string>  shareIds;
    if (getShareIds(pTmplObj->getTemplList(), shareIds))
    {
        dataStore.storeTemplLstShareInfo(pTmplObj->getAppId(),pTmplObj->getShrId(), shareIds, pTmplObj->getName());
        for (const std::string& shareId : shareIds)
        {
		std::cout << "archiving shareId=" << shareId << " " << __FILE__ << ":" << __LINE__ << std::endl;	
            if (ArchiveMsgCreator::createShareTemplLstMsg(archbuf, archlen, pTmplObj->getAppId(), false, std::stol(shareId), pTmplObj->getShrId(), pTmplObj->getName(),  32768))
                sendArchiveMsg(archbuf, archlen, 10);
        }
            std::vector<std::string> tokens;
            dataStore.getDeviceTkns(pTmplObj->getAppId(), shareIds, tokens);
	    if (tokens.size())
            	sendApplePush(tokens, pTmplObj->getName(), 1);
            std::vector<std::string> regIds;
            dataStore.getAndroidDeviceTkns(pTmplObj->getAppId(), shareIds, regIds);
	    if (regIds.size())
            	sendFirebaseMsg(pTmplObj->getAppId(), regIds, pTmplObj->getName());
	    std::cout << "Sending push notifications no of tokens=" << tokens.size() << " no of regIds=" << regIds.size() << " " << __FILE__ << ":" << __LINE__ << std::endl;	
            

    }
    char buf[1024];
    int mlen=0;
    if (getReply(buf, &mlen, SHARE_TEMPL_ITEM_RPLY_MSG))
    {
        sendMsg(buf, mlen, pTmplObj->getFd());
    }

    
    return;
}
