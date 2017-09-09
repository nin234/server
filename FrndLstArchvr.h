#ifndef _FRND_LST_ARCHVR_H_
#define _FRND_LST_ARCHVR_H_

#include <Archvr.h>
#include <HashMap.h>
#include <Constants.h>
#include <unordered_map>
#include <FrndLstMgr.h>

class FrndLstArchvr : public Archvr
{

		int frndFd;
		char wrbuf[BUF_SIZE_4K];
		std::unordered_map<long, long> frndLstIndx;	
		bool appendLst(long shareId, const char *buf, int len);

	public:
		FrndLstArchvr();
		~FrndLstArchvr();
		bool archiveMsg(const char *buf, int len);
		void populateFrndLstShIdMp(FrndLstMgr& frndMgr);
};
#endif
