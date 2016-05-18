#ifndef _PICTURE_SENDER_H_
#define _PICTURE_SENDER_H_

class PictureSender
{

	public:
		PictureSender();
		virtual ~PictureSender();
		void sendPictures();
		bool shouldEnqueMsg(int fd);
};
#endif
