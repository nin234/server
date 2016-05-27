#ifndef _OBSERVER_H_
#define _OBSERVER_H_

class Observer
{
	public:
		virtual bool notify(char *buf, int mlen, int fd) =0;
		virtual bool picDone(int fd) = 0;
		virtual ~Observer(){}
};

#endif
