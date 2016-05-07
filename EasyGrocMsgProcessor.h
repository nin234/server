#ifndef _EASYGROC_MSG_PROCESSOR_H_
#define _EASYGROC_MSG_PROCESSOR_H_

#include <MessageProcessor.h>
#include <array>
#include <memory>

class EasyGrocMsgProcessor : public MessageProcessor
{

	public:
	
		EasyGrocMsgProcessor();
		~EasyGrocMsgProcessor();
		void processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		
};
#endif
