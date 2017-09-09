#ifndef _FRND_LST_MGR_H_
#define _FRND_LST_MGR_H_

#include <string>
#include <HashMap.h>
#include <MessageObjs.h>
#include <array>

class FrndLstMgr
{
	FrndLstMgr();
	~FrndLstMgr();
	FrndLstMgr(const FrndLstMgr& fmgr) = delete;
	FrndLstMgr& operator=(const FrndLstMgr& fmgr) = delete;
	std::array<std::string, HASH_MAP_SIZE> frndLstShIdMp;
	std::array<std::mutex, HASH_MAP_SIZE> frndLstShIdMpMtx;
	
    public:
	static FrndLstMgr& Instance();
	void storeFrndLst(const FrndLstObj *pFrndObj);
	void storeFrndLst(long shareId, const std::string& frndLst);
	
};
#endif
