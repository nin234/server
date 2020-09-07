#ifndef _OBSERVER_H_
#define _OBSERVER_H_

class Observer
{
	public:
		virtual bool notify(char *buf, int mlen, int fd, bool *tryAgain) = 0;
		virtual bool picDone(int fd) = 0;
		virtual void updatePicShareInfo(int appId, long shareId, long frndShareId, const std::string& picName) = 0;
		virtual ~Observer(){}
};

#endif
