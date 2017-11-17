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
	bool createShareIdObj(char *buffer,  ssize_t mlen, int fd);
	bool createStoreIdObj(char *buffer,  ssize_t mlen, int fd);
	bool createFrndLstObj(char *buffer, ssize_t mlen, int fd);
	std::list<std::unique_ptr<MsgObj, MsgObjDeltr>> pMsgs;
	bool createTemplLstObj(char *buffer,  ssize_t mlen, int fd);
	bool createLstObj(char *buffer,  ssize_t mlen, int fd);
	bool createDeviceTknObj(char *buffer, ssize_t mlen, int fd);
	bool createGetItemObj(char *buffer, ssize_t mlen, int fd);
	bool createPicDoneObj(char *buffer, ssize_t mlen, int fd);
	bool createPicMetaDataObj(char *buffer, ssize_t mlen, int fd);
	bool createPicObj(char *buffer, ssize_t mlen, int fd);
    bool createShouldDownLoadObj(char *buffer, ssize_t mlen, int fd);

	std::map<int, PicObj*> picObjs;
	protected:
		void addMsgObj(std::unique_ptr<MsgObj, MsgObjDeltr> pMsg);

	public:
		MessageDecoder();
		virtual ~MessageDecoder();
		bool operator ()(char* buffer, ssize_t mlen, int fd);
		std::unique_ptr<MsgObj, MsgObjDeltr> getNextMsg();
		virtual bool decodeMsg(char *buffer, ssize_t mlen, int fd)=0;
		virtual int getAppId() =0;
};
#endif
