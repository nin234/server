#ifndef _SMARTMSG_DECODER_H_
#define _SMARTMSG_DECODER_H_

#include <MessageDecoder.h>

class SmartMsgDecoder : public MessageDecoder
{

	public:
		SmartMsgDecoder();
		virtual ~SmartMsgDecoder();
		bool decodeMsg(char *buffer, ssize_t mlen, int fd);
		int getAppId() {return SMARTMSG_ID;}
};
#endif
