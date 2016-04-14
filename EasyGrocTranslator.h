#ifndef _EASYGROC_TRANSLATOR_H_
#define _EASYGROC_TRANSLATOR_H_

#include <MessageTranslator.h>
#include <string>
#include <vector>
#include <memory>

class EasyGrocTranslator : public MessageTranslator
{
	public:
		EasyGrocTranslator();
		~EasyGrocTranslator();
		bool translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj);
};
#endif
