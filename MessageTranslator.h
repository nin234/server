#ifndef _MESSAGE_TRANSLATOR_H_
#define _MESSAGE_TRANSLATOR_H_

#include <MessageObjs.h>

class MessageTranslator
{
	public:
		MessageTranslator();
		virtual ~MessageTranslator();
		bool getShareIdReply(char *buf, int* mlen, long shareId);
		bool getStoreIdReply (char *buf, int* mlen);
		bool getReply (char *buf, int* mlen, int msgTyp);
		virtual bool translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj)=0;		
};
#endif
