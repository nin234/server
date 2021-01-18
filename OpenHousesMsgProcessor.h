#ifndef _OPENHOUSES_MSG_PROCESSOR_H_
#define _OPENHOUSES_MSG_PROCESSOR_H_

#include <MessageProcessor.h>

class OpenHousesMsgProcessor : public MessageProcessor
{

	public:
		OpenHousesMsgProcessor();
		~OpenHousesMsgProcessor();
		void processMsg( std::shared_ptr<MsgObj> pMsg);
}; 
#endif
