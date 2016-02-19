#ifndef _EASYGROC_MSG_PROCESSOR_H_
#define _EASYGROC_MSG_PROCESSOR_H_

#include <MessageProcessor.h>
#include <array>
#include <EasyGrocDataMgr.h>
#include <memory>

class EasyGrocMsgProcessor : public MessageProcessor
{

		std::array<int, NO_EASYGROC_MSGS> msgTypPrcsrs;
		void processTemplLstMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processLstMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processDeviceTknMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processGetItemMsg(const std::unique_ptr<MsgObj>& pMsg);
		EasyGrocDataMgr& dataStore;

	public:
	
		EasyGrocMsgProcessor();
		~EasyGrocMsgProcessor();
		void processMsg(const std::unique_ptr<MsgObj>& pMsg);
		
};
#endif
