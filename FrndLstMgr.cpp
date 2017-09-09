#include <FrndLstMgr.h>
#include <ArchiveMgr.h>

FrndLstMgr::FrndLstMgr()
{
	std::shared_ptr<FrndLstArchvr> pFrndArch = ArchiveMgr::Instance().getFrndLstArchvr(ARCHIVE_FRND_LST_MSG);
	if (pFrndArch)
		pFrndArch->populateFrndLstShIdMp(*this);
}

FrndLstMgr::~FrndLstMgr()
{

}

void
FrndLstMgr::storeFrndLst(const FrndLstObj *pFrndObj)
	
{
	std::lock_guard<std::mutex> lock(frndLstShIdMpMtx[pFrndObj->getShrId()]);
	frndLstShIdMp[pFrndObj->getShrId()] = pFrndObj->getFrndLst();
	return;
}

void
FrndLstMgr::storeFrndLst(long shareId, const std::string& frndLst)
	
{
	std::lock_guard<std::mutex> lock(frndLstShIdMpMtx[shareId]);
	frndLstShIdMp[shareId] = frndLst;
	return;
}


FrndLstMgr&
FrndLstMgr::Instance()
{
	static FrndLstMgr instance;
	return instance;
}
