#ifndef _FRND_LST_MGR_H_
#define _FRND_LST_MGR_H_

#include <string>
#include <HashMap.h>
#include <MessageObjs.h>

class FrndLstMgr
{
	FrndLstMgr();
	~FrndLstMgr();
	FrndLstMgr(const FrndLstMgr& fmgr) = delete;
	FrndLstMgr& operator=(const FrndLstMgr& fmgr) = delete;
	HashMap<long, std::string> frndLstShIdMp;
	
    public:
	static FrndLstMgr& Instance();
	void storeFrndLst(const FrndLstObj *pFrndObj);
	
};
#endif
