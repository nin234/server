#ifndef _MESSAGE_PROCESSOR_H_
#define _MESSAGE_PROCESSOR_H_

#include <ArchiveSndr.h>
#include <MessageDecoder.h>
#include <MessageTranslator.h>
#include <NtwIntf.h>
#include <array>
#include <Constants.h>
#include <ApplePush.h>
#include <FirebaseConnHdlr.h>
#include <CommonDataMgr.h>
#include <map>
#include <PictureSender.h>
#include <MessageEnqueuer.h>
#include <Observer.h>
#include <Util.h>

class MessageProcessor : public Observer
{
	std::shared_ptr<MessageDecoder> m_pDcd;
	std::shared_ptr<MessageTranslator> m_pTrnsl;
	std::shared_ptr<PictureSender> m_pPicSndr;
	std::unique_ptr<NtwIntf<MessageDecoder>>  pNtwIntf;
	std::unique_ptr<ArchiveSndr> pArch;
	std::unique_ptr<MessageEnqueuer> pMsgEnq;
	int maxFd;
	int nFds;
	std::array<int, NO_COMMON_MSGS> msgTypPrcsrs;
	std::shared_ptr<ApplePush> pAppleNotfy;
    std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify;
    
    void processGetItemPics(GetItemObj *pGetItemObj);

	protected:
		void sendArchiveMsg(const char *pMsg, size_t len, unsigned int msg_prio);
		bool sendMsg(char *buf, int mlen, int fd);
		void processArchvItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processDeviceTknMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processGetItemMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processPicMetaDataMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processPicMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg, int nMsgTyp);
		bool getShareIds(const std::string& lst, std::vector<std::string>& shareIds);
		bool getReply(char *buf, int *mlen, int msgTyp);
		CommonDataMgr& dataStore;

		
	public:
		MessageProcessor();
		virtual ~MessageProcessor();
		bool process();
		bool addFd(int fd);
		void setMaxFd(int mfd);
		void setDcdTransl(MessageDecoder *pDcd, MessageTranslator *pTrnsl);
		void processRequests();
		void processShareIdMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processStoreIdMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		void processFrndLstMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg);
		virtual void processMsg(const std::unique_ptr<MsgObj, MsgObjDeltr>& pMsg)=0;
		void setAppleNotify(std::shared_ptr<ApplePush> pAppleNtfy);
        void setFirebaseNotify(std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify);
		bool sendApplePush(const std::vector<std::string>& tokens, const std::string& msg, int badge);
        bool sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg);
		bool notify (char *buf, int mlen, int fd);
		bool picDone(int fd);
		void updatePicShareInfo(int appId, long shareId, long frndShareId, const std::string& picName); 
};

#endif
