#include <FrndLstMgr.h>
#include <ArchiveMgr.h>
#include <iostream>
#include <Util.h>
#include <sstream>


FrndLstMgr::FrndLstMgr()
{
}

FrndLstMgr::~FrndLstMgr()
{

}

bool
FrndLstMgr::returnEmptyFrndLst(const FrndLst& frndLst, int appId)
{
    switch (appId)
    {
        case OPENHOUSES_ID:
            if (frndLst.m_bUpdOpenHouses)
                return true;
        break;
        
        case AUTOSPREE_ID:
            if (frndLst.m_bUpdAutoSpree)
                return true;
        break;

        case EASYGROCLIST_ID:
            if (frndLst.m_bUpdEasyList)
                return true;
        break;

        default:
            return true;
        break;
    }
    return false;
}

std::string
FrndLstMgr::getFrndList(int appId, long shareId, bool bDontCheckUpdFlag)
{
    std::string frndLstEmpty;
    FrndLst frndLst;
    if (!m_frndLstDAO.get(shareId, frndLst))
        return frndLstEmpty;
    if (!bDontCheckUpdFlag)
    {
        if (returnEmptyFrndLst(frndLst, appId))
            return frndLstEmpty;
    } 

    std::stringstream frndLstStr;
    for (const auto& frnd : frndLst.m_bFrndLst )
    {
        frndLstStr << frnd.shareId << ":" << frnd.name << ";";
    }
    return frndLstStr.str();
}

bool
FrndLstMgr::updateStatus(long shareId, int appId)
{
    FrndLst frndLst;
    if (m_frndLstDAO.get(shareId, frndLst))
    {
       switch(appId)
       {
            case OPENHOUSES_ID:
            {
                frndLst.m_bUpdOpenHouses = true;
            }
            break;

            case AUTOSPREE_ID:
            {
                frndLst.m_bUpdAutoSpree = true;
            }
            break;
            
            case EASYGROCLIST_ID:
            {
                frndLst.m_bUpdEasyList = true;
            }
            break;

            default:
                return false;   
            break;
       } 
       return m_frndLstDAO.update(shareId, frndLst);
    }
    return false;
}


void
FrndLstMgr::storeFrndLst(const FrndLstObj *pFrndObj)
	
{
	std::cout << "Storing friendlist shareId=" << pFrndObj->getShrId() << " friendList=" << pFrndObj->getFrndLst() << " " << __FILE__ << ":" << __LINE__ << std::endl;	
    FrndLst frndLst;
    populateDAO(pFrndObj, frndLst);
    m_frndLstDAO.update(pFrndObj->getShrId(), frndLst);
	return;
}

void
FrndLstMgr::storeFrndLst(long shareId, const std::string& frndLst)
	
{
	std::cout << "Storing friendlist shareId=" << shareId << " friendList=" << frndLst << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	return;
}

void
FrndLstMgr::populateDAO(const FrndLstObj *pFrndObj, FrndLst& frndLst)
{
    switch (pFrndObj->getAppId())
    {
        case OPENHOUSES_ID:
            frndLst.m_bUpdOpenHouses = true;
        break;
        
        case AUTOSPREE_ID:
            frndLst.m_bUpdAutoSpree = true;
        break;

        case EASYGROCLIST_ID:
            frndLst.m_bUpdEasyList = true;
        break;

        default:
        break;
    }

    auto frndsVec = Util::split(pFrndObj->getFrndLst(), ';');

    for (const auto& frnd : frndsVec)
    {
         Frnd fnd;
        auto frndVec = Util::split(frnd, ':');
        if (!frndVec.size())
            continue;
        else if (frndVec.size() == 1)
        {
            fnd.name = " ";
            fnd.shareId = std::stol(frndVec[0]);
        }
        else
        {
            fnd.name = frndVec[1];
            fnd.shareId = std::stol(frndVec[0]);
        }
        frndLst.m_bFrndLst.push_back(fnd);
    }
    
}
