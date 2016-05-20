#ifndef _PICTURE_SENDER_H_
#define _PICTURE_SENDER_H_

#include <StorageElems.h>
#include <list>
#include <string>
#include <MessageTranslator.h>

class PictureSender
{
	std::list<shrIdLstName> picNamesShIds;
	std::shared_ptr<MessageTranslator> m_pTrnsl;
	public:
		PictureSender();
		virtual ~PictureSender();
		void sendPictures();
		bool shouldEnqueMsg(int fd);
		void insertPicNameShid(const shrIdLstName& shidlst);
		void setTrnsl(MessageTranslator *pTrnsl);
	
};
#endif
