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
#include <NtwIntfObserver.h>
#include <Util.h>
#include <Distributor.h>

class MessageProcessor : public Observer, public NtwIntfObserver
{
	std::shared_ptr<MessageDecoder> m_pDcd;
	std::shared_ptr<MessageTranslator> m_pTrnsl;
	std::shared_ptr<PictureSender> m_pPicSndr;
	std::unique_ptr<NtwIntf<MessageDecoder>>  pNtwIntf;
	std::unique_ptr<ArchiveSndr> pArch;
	std::unique_ptr<MessageEnqueuer> pMsgEnq;
    std::unique_ptr<Distributor> m_distributor;
	int maxFd;
	int nFds;
	std::shared_ptr<ApplePush> pAppleNotfy;
    std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify;
    
    void processGetItemPics(std::shared_ptr<GetItemObj> pGetItemObj);
    void sendUpdatedMaxShareIdIfRequd(std::shared_ptr<GetItemObj> pGetItemObj);
    void sendFrndLst(int appId, long shareId, int fd, bool bDontCheckUpdFlag);

	void sendPicNotifications(const std::vector<std::string>& shareIds, int appId, const std::string& picName);
	void processShareIdLocal(std::shared_ptr<MsgObj> pMsg);
	protected:
		void sendArchiveMsg(const char *pMsg, size_t len, unsigned int msg_prio);
		bool sendMsg(char *buf, int mlen, int fd);
		bool sendMsg(char *buf, int mlen, int fd, bool *tryAgain);
		void processArchvItemMsg(std::shared_ptr<MsgObj> pMsg);
		void processItemMsg(std::shared_ptr<MsgObj> pMsg);

		void processItemMsgDBInsert(std::shared_ptr<MsgObj> pMsg);

		void processDeviceTknMsg(std::shared_ptr<MsgObj> pMsg);
		void processGetItemMsgFromDB(std::shared_ptr<MsgObj> pMsg);
		void processPicMetaDataMsg(std::shared_ptr<MsgObj> pMsg);
		void processPicMsg(std::shared_ptr<MsgObj> pMsg);
		void processPicDoneMsg(std::shared_ptr<MsgObj> pMsg);
        void processShouldDownLoadMsg(std::shared_ptr<MsgObj> pMsg);
		void processMsg(std::shared_ptr<MsgObj> pMsg, int nMsgTyp);
		bool getShareIds(const std::string& lst, std::vector<std::string>& shareIds);
		bool getReply(char *buf, int *mlen, int msgTyp);
		CommonDataMgr& dataStore;

		
	public:
		MessageProcessor();
		virtual ~MessageProcessor();
		bool process();
		bool addFd(int fd);
		bool addSSLFd(int fd);
		void setMaxFd(int mfd);
		void setDcdTransl(MessageDecoder *pDcd, MessageTranslator *pTrnsl);
		void processRequests();
		void processShareIdMsg(std::shared_ptr<MsgObj> pMsg);
		void processStoreIdMsg(std::shared_ptr<MsgObj> pMsg);
		void processFrndLstMsg(std::shared_ptr<MsgObj> pMsg);
		virtual void processMsg(std::shared_ptr<MsgObj> pMsg)=0;
		void setAppleNotify(std::shared_ptr<ApplePush> pAppleNtfy);
        void setFirebaseNotify(std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify);
		bool sendApplePush(const std::vector<std::string>& tokens, const std::string& msg, int badge);
        bool sendFirebaseMsg(int appId, const std::vector<std::string>& tokens, const std::string& msg);
		bool notify (char *buf, int mlen, int fd, bool *tryAgain);

		bool picDone(int fd);
		void onCloseFd(int fd);
		void updatePicShareInfo(int appId, long shareId, long frndShareId, const std::string& picName); 
};

#endif
