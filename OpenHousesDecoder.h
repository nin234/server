#ifndef _OPENHOUSES_DECODER_H_
#define _OPENHOUSES_DECODER_H_

#include <MessageDecoder.h>

class OpenHousesDecoder : public MessageDecoder
{

	public:
		OpenHousesDecoder();
		virtual ~OpenHousesDecoder();
		bool decodeMsg(char *buffer, ssize_t mlen, int fd);
		int getAppId() {return OPENHOUSES_ID;}
};
#endif
