#ifndef _NTWINTF_OBSERVER_H_
#define _NTWINTF_OBSERVER_H_

class NtwIntfObserver
{

	public:
		virtual ~NtwIntfObserver(){}
		virtual void onCloseFd(int fd) = 0;
};

#endif
