#ifndef _AUTOSPREE_MSG_PROCESSOR_H_
#define _AUTOSPREE_MSG_PROCESSOR_H_

#include <MessageProcessor.h>
#include <ApplePush.h>

class AutoSpreeMsgProcessor : public MessageProcessor
{
	
	public:
		AutoSpreeMsgProcessor();
		~AutoSpreeMsgProcessor();
		void processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
};
#endif
