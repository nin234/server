#ifndef _SHARE_ID_ARCHVR_H_
#define _SHARE_ID_ARCHVR_H_

#include <Archvr.h>
#include <HashMap.h>
#include <MessageObjs.h>

class ShareIdArchvr : public Archvr
{

		int shareFd;
		long shareId;
		int shareTrnFd;
		bool archiveShareIdMsg(const char *buf, int len);
		bool archiveTrnIdShrIdMsg(const char *buf, int len);
	public:
	    ShareIdArchvr();
	    ~ShareIdArchvr();
	    bool archiveMsg(const char *buf, int len);
            long getShareId();
	    void populateTrnIdShrIdMp(HashMap<long, long>& trnIdShrIdMp);
};

#endif
