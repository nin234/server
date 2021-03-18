#include <OpenHousesMsgProcessor.h>
#include <OpenHousesDecoder.h>
#include <OpenHousesTranslator.h>
#include <iostream>
#include <Config.h>

OpenHousesMsgProcessor::OpenHousesMsgProcessor()
{
	std::cout << "Setting OpenHousesDecoder and OpenHousesTranslator and push notifys " << __FILE__ << ":" << __LINE__ << std::endl;	
	MessageProcessor::setDcdTransl(new OpenHousesDecoder(), new OpenHousesTranslator());
	
    pAppleNotifys[AppName::OPENHOUSES] = std::make_shared<ApplePush>("/home/ninan/certs/OpenHouses.p12", Config::Instance().sandBox(), Config::Instance().oHousesPasswd());
    pFirebaseNotifys[AppName::OPENHOUSES] = std::make_shared<FirebaseConnHdlr> ("1093967487031@gcm.googleapis.com", "AAAA_rWLIDc:APA91bHxlY5coSJxS5U7dfq6OSMgzgg5Dc2h-GBabkX5gaYmmQ15GxDVIhW6WzPdNh-tq1S9BPXYbJ5-cBE5BbJT5JUY2NDSzI_Sts4p6V2NujHSijFox-I5r6MwfP09IS9nRsdKSyV1");
}

OpenHousesMsgProcessor::~OpenHousesMsgProcessor()
{

}

void
OpenHousesMsgProcessor::processMsg( std::shared_ptr<MsgObj> pMsg)
{

	return;
}


bool 
OpenHousesMsgProcessor::sendApplePush(int appId, const std::vector<std::string>& tokens, const std::string& msg, int badge)
{
    auto pItr = pAppleNotifys.find(static_cast<AppName>(appId));
    if (pItr != pAppleNotifys.end())
    {
        return pItr->second->send(tokens, msg, badge);
    }
    return true;
}

bool 
OpenHousesMsgProcessor::sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg)
{

    std::string fbmsg = "OpenHouses- ";
    fbmsg += msg;
    auto pItr = pFirebaseNotifys.find(static_cast<AppName>(appId));
    if (pItr != pFirebaseNotifys.end())
    {
        return pItr->second->send(tokens, fbmsg);
    }
    return true;
}

void 
OpenHousesMsgProcessor::getSendEvents()
{
    for (const auto& [key, pFirebaseNotify] : pFirebaseNotifys)
    {
        pFirebaseNotify->getSendEvents();
    }
}


