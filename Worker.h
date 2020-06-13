#ifndef _WORKER_H_
#define _WORKER_H_

#include <set>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <memory>
#include <MessageProcessor.h>
#include <ApplePush.h>
#include <FirebaseConnHdlr.h>

class Worker
{

	std::unique_ptr<MessageProcessor> m_pMsgProcessor;

	public:

	Worker();
	virtual ~Worker();
	static void *entry(void *context);
	void *main();
	bool addFd(int fd);
	bool addSSLFd(int fd);
	void setMaxFd(int mfd);
	virtual void setMaxFd()=0;
    virtual void setMsgProcessors(std::shared_ptr<ApplePush> pApplPushNotfy, std::shared_ptr<FirebaseConnHdlr> pFirebaseNotify)=0;
	void setMsgProcessor(MessageProcessor *pMsgProcessor);
};
#endif
