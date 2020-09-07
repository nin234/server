#ifndef _PICTURE_SENDER_H_
#define _PICTURE_SENDER_H_

#include <StorageElems.h>
#include <list>
#include <string>
#include <MessageTranslator.h>
#include <Observer.h>
#include <vector>
#include <iostream>

struct PicFileDetails
{
	int picFd;
	int picLen;
	int totWritten;
	int shareId;
	int frndShareId;
	int appId;
	int picSoFar;
    bool waiting;
    long tv_sec;
	std::string picName;
	std::string picRealName;
    bool tryAgain;
    long tryAgainTime;	
};

std::ostream& operator << (std::ostream& os, const PicFileDetails& pfd);

class PictureSender
{
	std::list<shrIdLstName> picNamesShIds;
	std::shared_ptr<MessageTranslator> m_pTrnsl;
	Observer *m_pObs;
	std::map<int, PicFileDetails> picFdMp;
	void sendPicMetaDat();	
	void sendPicData();
	void closeAndNotify(int picFd, int ntwFd, std::map<int, PicFileDetails>::iterator& pItr);
	public:
		PictureSender();
		virtual ~PictureSender();
		void sendPictures();
		bool shouldEnqueMsg(int fd);
		void insertPicNameShid(const shrIdLstName& shidlst);
		void setTrnsl(MessageTranslator *pTrnsl);
		void attach (Observer *pObs);
   	    void updateWaitingInfo(const std::string& picName, long shareId, bool bDownLoad);
		std::vector<PicFileDetails> onCloseNtwFd(int ntwFd);
	
};
#endif
