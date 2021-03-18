#ifndef _AUTOSPREE_MSG_PROCESSOR_H_
#define _AUTOSPREE_MSG_PROCESSOR_H_

#include <MessageProcessor.h>
#include <unordered_map>

class AutoSpreeMsgProcessor : public MessageProcessor
{
    std::unordered_map<AppName, std::shared_ptr<ApplePush>> pAppleNotifys;
    std::unordered_map<AppName, std::shared_ptr<FirebaseConnHdlr>> pFirebaseNotifys;
	
	public:
		AutoSpreeMsgProcessor();
		~AutoSpreeMsgProcessor();
		void processMsg(std::shared_ptr<MsgObj> pMsg);
		bool sendApplePush(int appId, const std::vector<std::string>& tokens, const std::string& msg, int badge);
        bool sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg);
        void getSendEvents();
};
#endif
