#ifndef _AUTOSPREE_TRANSLATOR_H_
#define _AUTOSPREE_TRANSLATOR_H_

#include <MessageTranslator.h>

class AutoSpreeTranslator : public MessageTranslator
{

	public:
		AutoSpreeTranslator();
		~AutoSpreeTranslator();
		bool translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj);

};
#endif

