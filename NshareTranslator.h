#ifndef _NSHARE_TRANSLATOR_H_
#define _NSHARE_TRANSLATOR_H_

#include <MessageTranslator.h>
#include <string>
#include <vector>
#include <memory>

class NshareTranslator : public MessageTranslator
{
	public:
		NshareTranslator();
		~NshareTranslator();
		bool translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj);
};
#endif
