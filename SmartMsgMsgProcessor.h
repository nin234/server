#ifndef _SMARTMSG_MSG_PROCESSOR_H_
#define _SMARTMSG_MSG_PROCESSOR_H_

#include <MessageProcessor.h>

class SmartMsgMsgProcessor : public MessageProcessor
{

	public:
		SmartMsgMsgProcessor();
		~SmartMsgMsgProcessor();
		void processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
}; 
#endif
