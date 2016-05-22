#ifndef _PICTURE_SENDER_H_
#define _PICTURE_SENDER_H_

#include <StorageElems.h>
#include <list>
#include <string>
#include <MessageTranslator.h>
#include <Observer.h>

struct PicFileDetails
{
	int picFd;
	int picLen;
	int totWritten;
	
};

class PictureSender
{
	std::list<shrIdLstName> picNamesShIds;
	std::shared_ptr<MessageTranslator> m_pTrnsl;
	Observer *m_pObs;
	std::map<int, PicFileDetails> picFdMp;
	void sendPicMetaDat();	
	void sendPicData();

	public:
		PictureSender();
		virtual ~PictureSender();
		void sendPictures();
		bool shouldEnqueMsg(int fd);
		void insertPicNameShid(const shrIdLstName& shidlst);
		void setTrnsl(MessageTranslator *pTrnsl);
		void attach (Observer *pObs);
	
};
#endif
