#ifndef _EASYGROC_DECODER_H_
#define _EASYGROC_DECODER_H_

#include <MessageDecoder.h>

class EasyGrocDecoder : public MessageDecoder
{

std::array<int, NO_EASYGROC_MSGS> msgTypPrcsrs;
    bool createShareTemplLstObj(char *buffer,  ssize_t mlen, int fd);
    
	public:
		EasyGrocDecoder();
		virtual ~EasyGrocDecoder();
		bool decodeMsg(char *buffer, ssize_t mlen, int fd);
		int getAppId() {return EASYGROCLIST_ID;}
};
#endif
