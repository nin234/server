#include <FrndLstMgr.h>
#include <ArchiveMgr.h>
#include <iostream>

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
	std::cout << "Storing friendlist shareId=" << pFrndObj->getShrId() << " friendList=" << pFrndObj->getFrndLst() << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	frndLstShIdMp[pFrndObj->getShrId()] = pFrndObj->getFrndLst();
	return;
}

void
FrndLstMgr::storeFrndLst(long shareId, const std::string& frndLst)
	
{
	std::lock_guard<std::mutex> lock(frndLstShIdMpMtx[shareId]);
	std::cout << "Storing friendlist shareId=" << shareId << " friendList=" << frndLst << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	frndLstShIdMp[shareId] = frndLst;
	return;
}


FrndLstMgr&
FrndLstMgr::Instance()
{
	static FrndLstMgr instance;
	return instance;
}
