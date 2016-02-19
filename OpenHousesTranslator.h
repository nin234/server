#ifndef _OPENHOUSES_TRANSLATOR_H_
#define _OPENHOUSES_TRANSLATOR_H_

#include <MessageTranslator.h>

class OpenHousesTranslator : public MessageTranslator
{
	public:
		OpenHousesTranslator();
		virtual ~OpenHousesTranslator();
		bool translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj);

};
#endif
