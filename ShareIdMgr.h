#ifndef _SHARE_ID_MGR_H_
#define _SHARE_ID_MGR_H_

#include <string>
#include <map>
#include <HashMap.h>
#include <mutex>
#include <LckFreeLst.h>
#include <ShareIdDAO.h>

class ShareIdMgr
{
		ShareIdMgr();
		~ShareIdMgr();
		ShareIdMgr(const ShareIdMgr& sMgr) = delete;
		ShareIdMgr& operator=(const ShareIdMgr& sMgr) = delete;
//		HashMap<long, long> trnIdShrIdMp;
		int shareIdStep;
		long shareId;
		long shareId_Init;
		std::mutex shid_init_mutex;
        ShareIdDAO m_shareIdDAO;

	public:
		static ShareIdMgr&  Instance();
		long getShareId(long trnId, bool& archive, const std::string& deviceId);
		void setShareId(long sid);		
		void setShareIdStep(int step);
		void initThreadShareId();
		void storeTrndId(long trnId, long shrId, bool& archive);

};
#endif
