#include <ShareIdMgr.h>
#include <ArchiveMgr.h>
#include <Constants.h>
#include <iostream>
#include <ShareIdDAO.h>

ShareIdMgr::ShareIdMgr()
{
	std::shared_ptr<ShareIdArchvr> pShArch = ArchiveMgr::Instance().getShareIdArchvr(ARCHIVE_SHARE_ID_MSG);
	if (pShArch)
	{
		shareId_Init = pShArch->getShareId();
	}
	else
		shareId_Init =1000;
	//pShArch->populateTrnIdShrIdMp(trnIdShrIdMp);
	++shareId_Init;
}

ShareIdMgr::~ShareIdMgr()
{

}

void
ShareIdMgr::initThreadShareId()
{
	std::lock_guard<std::mutex> lock(shid_init_mutex);
	shareId = shareId_Init;
	std::cout << "Setting shareId to " << shareId << " " << __FILE__ << " " << __LINE__ << std::endl;
	return;
}

ShareIdMgr&
ShareIdMgr::Instance()
{
	
	static ShareIdMgr instance;
	return instance;
}

long
ShareIdMgr::getShareId(long trnId, bool& archive, const std::string& deviceId)
{
	std::lock_guard<std::mutex> lock(shid_init_mutex);
	archive = true;
    if (deviceId.size())
    {
        long shid = m_shareIdDAO.get(deviceId);
        if (shid)
            return shid;

        ++shareId;
        m_shareIdDAO.set(deviceId, shareId);
        return shareId;
    }
	++shareId;
	std::cout << "Returning new shareId=" << shareId << " " << __FILE__ << " " << __LINE__ << std::endl;
	return shareId;
}

void
ShareIdMgr::storeTrndId(long trnId, long shrId, bool& archive)
{
/*	
	if (trnIdShrIdMp.getValue(trnId, shrId))		
	{
		archive = false;
		return;
	}
	archive = true;
	trnIdShrIdMp[trnId] = shrId;
*/
	return;
}


void
ShareIdMgr::setShareId(long sid)
{
	shareId = sid + shareId_Init;
	return;
}

void
ShareIdMgr::setShareIdStep(int step)
{
	shareIdStep = step;
	std::cout << "Setting shareIdStep to " << shareIdStep << " " << __FILE__ << " " << __LINE__ << std::endl;
	return;
}
