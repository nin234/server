#ifndef _FRND_LST_MGR_H_
#define _FRND_LST_MGR_H_

#include <string>
#include <HashMap.h>
#include <MessageObjs.h>
#include <array>

class FrndLstMgr
{
	
  public:
	FrndLstMgr();
	~FrndLstMgr();
	void storeFrndLst(const FrndLstObj *pFrndObj);
	void storeFrndLst(long shareId, const std::string& frndLst);
	
};
#endif
