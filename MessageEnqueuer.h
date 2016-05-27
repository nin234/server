#ifndef _MESSAGE_ENQUEUER_H_
#define _MESSAGE_ENQUEUER_H_

#include <list>
#include <map>
#include <memory>
#include <MessageObjs.h>

	class MessageEnqueuer
	{
			std::map<int, std::list<std::unique_ptr<MsgObj, MsgObjDeltr>>> pMsgsFdMp;

		public:
			MessageEnqueuer();
			~MessageEnqueuer();
			void enqMsg(std::unique_ptr<MsgObj, MsgObjDeltr> pMsg);
			std::unique_ptr<MsgObj, MsgObjDeltr> getNextMsg(int fd);
	};
#endif
