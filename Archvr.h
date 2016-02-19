#ifndef _ARCHVR_H_
#define _ARCHVR_H_

class Archvr
{
	public:
		Archvr() {};
		virtual ~Archvr() {};
		virtual bool archiveMsg(const char *buf, int len)=0;
};
#endif
