#ifndef _AUTOSPREE_DECODER_H_
#define _AUTOSPREE_DECODER_H_

#include <MessageDecoder.h>

class AutoSpreeDecoder : public MessageDecoder
{

	public:
		AutoSpreeDecoder();
		virtual ~AutoSpreeDecoder();
		bool decodeMsg(char *buffer, ssize_t mlen, int fd);
};
#endif
