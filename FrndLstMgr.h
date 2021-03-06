#ifndef _FRND_LST_MGR_H_
#define _FRND_LST_MGR_H_

#include <string>
#include <HashMap.h>
#include <MessageObjs.h>
#include <array>
#include <FrndLstDAO.h>

class FrndLstMgr
{
    FrndLstDAO m_frndLstDAO;
    void populateDAO(std::shared_ptr<FrndLstObj> pFrndObj, FrndLst& frndLst);

    bool returnEmptyFrndLst(const FrndLst& frndLst, int appId);
	
  public:
	FrndLstMgr();
	~FrndLstMgr();
	void storeFrndLst(std::shared_ptr<FrndLstObj> pFrndObj);
	void storeFrndLst(long shareId, const std::string& frndLst);


    std::string getFrndList(int appId, long shareId, bool bDontCheckUpdFlag);
    bool updateStatus(long shareId, int appId);
	
};
#endif
