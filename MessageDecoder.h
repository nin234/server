#ifndef _MESSAGE_DECODER_H_
#define _MESSAGE_DECODER_H_

#include <Constants.h>
#include <unistd.h>
#include <MessageObjs.h>
#include <list>
#include <memory>
#include <array>

class MessageDecoder
{
	bool createShareIdObj(char *buffer,  ssize_t mlen, int fd);
	bool createStoreIdObj(char *buffer,  ssize_t mlen, int fd);
	bool createFrndLstObj(char *buffer, ssize_t mlen, int fd);
	std::list<std::unique_ptr<MsgObj>> pMsgs;
	std::array<int, NO_COMMON_MSGS> msgTypPrcsrs;
	bool createTemplLstObj(char *buffer,  ssize_t mlen, int fd);
	bool createLstObj(char *buffer,  ssize_t mlen, int fd);
	bool createDeviceTknObj(char *buffer, ssize_t mlen, int fd);
	bool createGetItemObj(char *buffer, ssize_t mlen, int fd);

	protected:
		void addMsgObj(std::unique_ptr<MsgObj> pMsg);

	public:
		MessageDecoder();
		virtual ~MessageDecoder();
		bool operator ()(char* buffer, ssize_t mlen, int fd);
		std::unique_ptr<MsgObj> getNextMsg();
		virtual bool decodeMsg(char *buffer, ssize_t mlen, int fd)=0;
		virtual int getAppId() =0;
};
#endif
