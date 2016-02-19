#include <ShareIdMgr.h>
#include <ArchiveMgr.h>
#include <Constants.h>

ShareIdMgr::ShareIdMgr()
{
	std::shared_ptr<ShareIdArchvr> pShArch = ArchiveMgr::Instance().getShareIdArchvr(ARCHIVE_SHARE_ID_MSG);
	if (pShArch)
	{
		shareId_Init = pShArch->getShareId();
	}
	else
		shareId_Init =0;
	pShArch->populateTrnIdShrIdMp(trnIdShrIdMp);
}

ShareIdMgr::~ShareIdMgr()
{

}

void
ShareIdMgr::initThreadShareId()
{
	std::lock_guard<std::mutex> lock(shid_init_mutex);
	++shareId_Init;
	shareId = shareId_Init;
	return;
}

thread_local long
ShareIdMgr::shareId=0;

ShareIdMgr&
ShareIdMgr::Instance()
{
	
	static ShareIdMgr instance;
	return instance;
}

long
ShareIdMgr::getShareId(long trnId, bool& archive)
{
	long val;
	if (trnIdShrIdMp.getValue(trnId, val))	
	{	
		archive = false;
		return val;
	}
	archive = true;
	shareId += shareIdStep;
	trnIdShrIdMp[trnId] = shareId;
	return shareId;
}

void
ShareIdMgr::storeTrndId(long trnId, long shrId, bool& archive)
{
	
	if (trnIdShrIdMp.getValue(trnId, shrId))		
	{
		archive = false;
		return;
	}
	archive = true;
	trnIdShrIdMp[trnId] = shrId;
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
	return;
}
