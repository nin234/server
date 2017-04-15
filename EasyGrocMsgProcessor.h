#ifndef _EASYGROC_MSG_PROCESSOR_H_
#define _EASYGROC_MSG_PROCESSOR_H_

#include <MessageProcessor.h>
#include <array>
#include <memory>

class EasyGrocMsgProcessor : public MessageProcessor
{

	std::array<int, NO_EASYGROC_MSGS> msgTypPrcsrs;


	public:
	
		EasyGrocMsgProcessor();
		~EasyGrocMsgProcessor();
		void processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
    void processTemplItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		
};
#endif
