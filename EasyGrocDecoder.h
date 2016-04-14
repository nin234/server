#ifndef _EASYGROC_DECODER_H_
#define _EASYGROC_DECODER_H_

#include <MessageDecoder.h>

class EasyGrocDecoder : public MessageDecoder
{


	public:
		EasyGrocDecoder();
		virtual ~EasyGrocDecoder();
		bool decodeMsg(char *buffer, ssize_t mlen, int fd);
		int getAppId() {return 3;}
};
#endif
