#ifndef _MESSAGE_PROCESSOR_H_
#define _MESSAGE_PROCESSOR_H_

#include <ArchiveSndr.h>
#include <MessageDecoder.h>
#include <MessageTranslator.h>
#include <NtwIntf.h>
#include <array>
#include <Constants.h>
#include <ApplePush.h>
#include <CommonDataMgr.h>
#include <map>

class MessageProcessor
{
	std::shared_ptr<MessageDecoder> m_pDcd;
	std::unique_ptr<NtwIntf<MessageDecoder>>  pNtwIntf;
	std::unique_ptr<ArchiveSndr> pArch;
	int maxFd;
	int nFds;
	std::array<int, NO_COMMON_MSGS> msgTypPrcsrs;
	std::shared_ptr<ApplePush> pAppleNotfy;	
	std::map<int, std::unique_ptr<MsgObj>> fdPicMetaMp;	
	protected:
		void sendArchiveMsg(const char *pMsg, size_t len, unsigned int msg_prio);
		MessageTranslator *m_pTrnsl;
		bool sendMsg(char *buf, int mlen, int fd);
		void processArchvItemMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processItemMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processDeviceTknMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processGetItemMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processPicMetaDataMsg(const std::unique_ptr<MsgObj>& pMsg);
		CommonDataMgr& dataStore;


	public:
		MessageProcessor();
		virtual ~MessageProcessor();
		bool process();
		bool addFd(int fd);
		void setMaxFd(int mfd);
		void setDcdTransl(MessageDecoder *pDcd, MessageTranslator *pTrnsl);
		void processRequests();
		void processShareIdMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processStoreIdMsg(const std::unique_ptr<MsgObj>& pMsg);
		void processFrndLstMsg(const std::unique_ptr<MsgObj>& pMsg);
		virtual void processMsg(const std::unique_ptr<MsgObj>& pMsg)=0;
		void setAppleNotify(std::shared_ptr<ApplePush> pAppleNtfy);
		bool sendApplePush(const std::vector<std::string>& tokens, const std::string& msg, int badge);
};
#endif
