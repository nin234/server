#ifndef _MESSAGE_TRANSLATOR_H_
#define _MESSAGE_TRANSLATOR_H_

#include <MessageObjs.h>
#include <memory>
#include <StorageElems.h>

class MessageTranslator
{
	public:
		MessageTranslator();
		virtual ~MessageTranslator();
		bool getShareIdReply(char *buf, int* mlen, long shareId);
		bool getShareIdRemoteHostMsg(char *buf, int* mlen, std::string host, int port);
		bool getUpdateShareIdMsg(char *buf, int* mlen, long shareId);
		bool getStoreIdReply (char *buf, int* mlen);
		bool getReply (char *buf, int* mlen, int msgTyp);
		bool getShareIds(const std::string& lst, std::vector<std::string>& shareIds);
		bool getListMsg(char *buf, int *mlen, int buflen, const std::string& name, const std::string& lst, int msgId, long shareId);
		bool getPicMetaMsg(char *buf, int *mlen, int buflen, const shrIdLstName& shidlst);
		bool getShouldUploadMsg(char *pMsg, int *mlen, const PicMetaDataObj *pPicMetaObj, bool shoudUpload, int picOffset);
		virtual bool translateMsg(char *buf, int *mlen, int msgTyp, MsgObj *pMsgObj)=0;		
        
        bool getFrndLstMsg(char *buf, int *mlen, int buflen, const std::string& frndLst);
        bool getTotalPicLenMsg(char *buf, int *mlen, long picLen);
};
#endif
