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
		std::mutex shid_init_mutex;
        ShareIdDAO m_shareIdDAO;

	public:
		static ShareIdMgr&  Instance();
		long getShareId(const std::string& deviceId);
        long readShareId() {return m_shareIdDAO.getMaxShareId();}

};
#endif
