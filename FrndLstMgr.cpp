#include <FrndLstMgr.h>
#include <ArchiveMgr.h>

FrndLstMgr::FrndLstMgr()
{
	std::shared_ptr<FrndLstArchvr> pFrndArch = ArchiveMgr::Instance().getFrndLstArchvr(ARCHIVE_FRND_LST_MSG);
	if (pFrndArch)
		pFrndArch->populateFrndLstShIdMp(frndLstShIdMp);
}

FrndLstMgr::~FrndLstMgr()
{

}

void
FrndLstMgr::storeFrndLst(const FrndLstObj *pFrndObj)
	
{
	frndLstShIdMp[pFrndObj->getShrId()] = pFrndObj->getFrndLst();

	return;
}

FrndLstMgr&
FrndLstMgr::Instance()
{
	static FrndLstMgr instance;
	return instance;
}
