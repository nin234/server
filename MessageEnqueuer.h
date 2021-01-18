#ifndef _MESSAGE_ENQUEUER_H_
#define _MESSAGE_ENQUEUER_H_

#include <list>
#include <map>
#include <memory>
#include <MessageObjs.h>

	class MessageEnqueuer
	{
			std::map<int, std::list<std::shared_ptr<MsgObj>>> pMsgsFdMp;

		public:
			MessageEnqueuer();
			~MessageEnqueuer();
			void enqMsg(std::shared_ptr<MsgObj> pMsg);
			std::shared_ptr<MsgObj> getNextMsg(int fd);
	};
#endif
