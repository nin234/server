#ifndef _SMARTMSG_MSG_PROCESSOR_H_
#define _SMARTMSG_MSG_PROCESSOR_H_

#include <MessageProcessor.h>

class SmartMsgMsgProcessor : public MessageProcessor
{

	public:
		SmartMsgMsgProcessor();
		~SmartMsgMsgProcessor();
		void processMsg(std::shared_ptr<MsgObj> pMsg);
		bool sendApplePush(int appId, const std::vector<std::string>& tokens, const std::string& msg, int badge);
        bool sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg);
        void getSendEvents();
}; 
#endif
