#ifndef _SMARTMSG_TRANSLATOR_H_
#define _SMARTMSG_TRANSLATOR_H_

#include <MessageTranslator.h>

class SmartMsgTranslator : public MessageTranslator
{
	public:
		SmartMsgTranslator();
		virtual ~SmartMsgTranslator();
		bool translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj);

};
#endif
