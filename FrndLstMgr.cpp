#include <FrndLstMgr.h>
#include <ArchiveMgr.h>
#include <iostream>

FrndLstMgr::FrndLstMgr()
{
}

FrndLstMgr::~FrndLstMgr()
{

}

void
FrndLstMgr::storeFrndLst(const FrndLstObj *pFrndObj)
	
{
	std::cout << "Storing friendlist shareId=" << pFrndObj->getShrId() << " friendList=" << pFrndObj->getFrndLst() << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return;
}

void
FrndLstMgr::storeFrndLst(long shareId, const std::string& frndLst)
	
{
	std::cout << "Storing friendlist shareId=" << shareId << " friendList=" << frndLst << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return;
}


