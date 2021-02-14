#ifndef _MESSAGE_DECODER_H_
#define _MESSAGE_DECODER_H_

#include <Constants.h>
#include <unistd.h>
#include <MessageObjs.h>
#include <list>
#include <memory>
#include <array>
#include <map>
#include <functional>

class MessageDecoder
{
	std::list<std::shared_ptr<MsgObj>> pMsgs;

	bool createShareIdObj(char *buffer,  ssize_t mlen, int fd);
	bool createStoreIdObj(char *buffer,  ssize_t mlen, int fd);
	bool createFrndLstObj(char *buffer, ssize_t mlen, int fd);
	bool createTemplLstObj(char *buffer,  ssize_t mlen, int fd);
	bool createLstObj(char *buffer,  ssize_t mlen, int fd);
	bool createDeviceTknObj(char *buffer, ssize_t mlen, int fd);
	bool createGetItemObj(char *buffer, ssize_t mlen, int fd);
	bool createPicDoneObj(char *buffer, ssize_t mlen, int fd);
	bool createPicMetaDataObj(char *buffer, ssize_t mlen, int fd);
	bool createPicObj(char *buffer, ssize_t mlen, int fd);

    bool createShouldDownLoadObj(char *buffer, ssize_t mlen, int fd);

    bool createGetRemoteHost(char *buffer, ssize_t mlen, int fd);

	protected:
		void addMsgObj(std::shared_ptr<MsgObj> pMsg);

	public:
		MessageDecoder();
		virtual ~MessageDecoder();
		bool operator ()(char* buffer, ssize_t mlen, int fd);
		std::shared_ptr<MsgObj> getNextMsg();
		virtual bool decodeMsg(char *buffer, ssize_t mlen, int fd)=0;
		virtual int getAppId() =0;
};
#endif
