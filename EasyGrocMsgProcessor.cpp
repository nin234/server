#include <EasyGrocMsgProcessor.h>
#include <EasyGrocDecoder.h>
#include <EasyGrocTranslator.h>
#include <functional>
#include <iostream>
#include <MessageObjs.h>
#include <ArchiveMsgCreator.h>
#include <vector>
#include <sstream>
#include <Config.h>

using namespace std::placeholders;

EasyGrocMsgProcessor::EasyGrocMsgProcessor()
{
    std::cout << "Setting EasyGrocDecoder and EasyGrocTranslator and Apple Push and Firebase" << " " << __FILE__ << ":" << __LINE__ << std::endl;   
	MessageProcessor::setDcdTransl(new EasyGrocDecoder(), new EasyGrocTranslator());
	pAppleNotifys[AppName::EASYGROCLIST] = std::make_shared<ApplePush>("/home/ninan/certs/EasyGrocList.p12", Config::Instance().sandBox(), Config::Instance().eGrocPasswd());
	pAppleNotifys[AppName::NSHARELIST] = std::make_shared<ApplePush>("/home/ninan/certs/nsharelist.p12", Config::Instance().sandBox(), Config::Instance().eGrocPasswd());
    pFirebaseNotifys[AppName::EASYGROCLIST] = std::make_shared<FirebaseConnHdlr> ("977230809988@gcm.googleapis.com", "AAAA44d-y4Q:APA91bHzCm1pxTgFjsQValZwOKmNejF4xd5ImJrEsSPe5VTgAFXN5BBPXG9BCS6Tj2zVnJtotfycf2vrM1n7GRd3-AwACd7-A1HkxOTjGhxHEKnN7w2zzgCcewJQylHqL3GGJElYAjoa");
    pFirebaseNotifys[AppName::NSHARELIST] = std::make_shared<FirebaseConnHdlr> ("629527144642@gcm.googleapis.com", "AAAAkpK96MI:APA91bHw239pOKFgL_p9AuJhlvTdaBVOrFWNE5x050_V48cpYhZ14LzMeVTOXXQIDV7a5k-eH4TBqMCr6SPoqOw0S8TpZlWJ-DMQo3xWtOPr1cUwLQiZfuVQMgl-CXV3Gj_x6cG9w-qb");
    
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
            	sendApplePush(pTmplObj->getAppId(), tokens, pTmplObj->getName(), 1);
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


bool 
EasyGrocMsgProcessor::sendApplePush(int appId, const std::vector<std::string>& tokens, const std::string& msg, int badge)
{

    auto pItr = pAppleNotifys.find(static_cast<AppName>(appId));
    if (pItr != pAppleNotifys.end())
    {
        return pItr->second->send(tokens, msg, badge);
    }
    return true;
}

void 
EasyGrocMsgProcessor::getSendEvents()
{
    for (const auto& [key, pFirebaseNotify] : pFirebaseNotifys)
    {
        pFirebaseNotify->getSendEvents();
    }
}

bool 
EasyGrocMsgProcessor::sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg)
{

    std::string fbmsg;
    
    switch(appId)
	{
		case NSHARELIST_ID:
			fbmsg = "nsharelist- ";
		break;

		case EASYGROCLIST_ID:
			fbmsg = "EasyGrocList- ";
		break;

		default:
		break;
	}

    fbmsg += msg;
    auto pItr = pFirebaseNotifys.find(static_cast<AppName>(appId));
    if (pItr != pFirebaseNotifys.end())
    {
        return pItr->second->send(tokens, fbmsg);
    }
    else
    {
        std::cout << "Failed to send firebase message, No firebase handler for appId=" << appId << " " << __FILE__ << ":" << __LINE__ << std::endl; 
        return false;
    }
    return true;
}

