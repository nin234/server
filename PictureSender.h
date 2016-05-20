#ifndef _PICTURE_SENDER_H_
#define _PICTURE_SENDER_H_

#include <StorageElems.h>
#include <list>
#include <string>

class PictureSender
{
	std::list<shrIdLstName> picNamesShIds;

	public:
		PictureSender();
		virtual ~PictureSender();
		void sendPictures();
		bool shouldEnqueMsg(int fd);
		void insertPicNameShid(const shrIdLstName& shidlst);
	
};
#endif
