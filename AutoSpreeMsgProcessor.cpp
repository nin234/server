#include <AutoSpreeMsgProcessor.h>
#include <AutoSpreeDecoder.h>
#include <AutoSpreeTranslator.h>
#include <iostream>
#include <Config.h>

AutoSpreeMsgProcessor::AutoSpreeMsgProcessor()
{
	MessageProcessor::setDcdTransl(new AutoSpreeDecoder(), new AutoSpreeTranslator());
	pAppleNotifys[AppName::AUTOSPREE] = std::make_shared<ApplePush>("/home/ninan/certs/AutoSpree.p12", Config::Instance().sandBox(), Config::Instance().aSpreePasswd());
    pFirebaseNotifys[AppName::AUTOSPREE] = std::make_shared<FirebaseConnHdlr> ("516330867276@gcm.googleapis.com", "AAAAeDe3ykw:APA91bFNNEhYWFvI05qNo2xrd2C4w_dZN6dBtrvUJHjY3qs6LLFtoridHfO5G4723SzlqF54fYfp99MbDamVk80g7J19vN9rCzVsTiE3IbU4vszKHerRItPGI8Ng1wHKgyZF3-AkGJ71");
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

bool 
AutoSpreeMsgProcessor::sendApplePush(int appId, const std::vector<std::string>& tokens, const std::string& msg, int badge)
{

    auto pItr = pAppleNotifys.find(static_cast<AppName>(appId));
    if (pItr != pAppleNotifys.end())
    {
        return pItr->second->send(tokens, msg, badge);
    }
    return true;
}

bool 
AutoSpreeMsgProcessor::sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg)
{
    std::string fbmsg = "AutoSpree- ";
    fbmsg += msg;
    auto pItr = pFirebaseNotifys.find(static_cast<AppName>(appId));
    if (pItr != pFirebaseNotifys.end())
    {
        return pItr->second->send(tokens, fbmsg);
    }
    return true;
}

void 
AutoSpreeMsgProcessor::getSendEvents()
{
    for (const auto& [key, pFirebaseNotify] : pFirebaseNotifys)
    {
        pFirebaseNotify->getSendEvents();
    }
}


