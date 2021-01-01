#include <ShareIdMgr.h>
#include <ArchiveMgr.h>
#include <Constants.h>
#include <iostream>
#include <ShareIdDAO.h>
#include <DistribMgr.h>
#include <Config.h>

ShareIdMgr::ShareIdMgr()
{
}

ShareIdMgr::~ShareIdMgr()
{

}

ShareIdMgr&
ShareIdMgr::Instance()
{
	
	static ShareIdMgr instance;
	return instance;
}

long
ShareIdMgr::getShareId(const std::string& deviceId)
{
	std::lock_guard<std::mutex> lock(shid_init_mutex);
    long shareId = m_shareIdDAO.getMaxShareId();
    if (!shareId)
    {
       shareId = Config::Instance().getStartShareId();
       if (!shareId)
       {
            return 0;
       }
    }

    if (deviceId.size())
    {
        long shid = m_shareIdDAO.get(deviceId);
        if (shid)
            return shid;
        ++shareId;
        m_shareIdDAO.set(deviceId, shareId);
        m_shareIdDAO.setMaxShareId(shareId);
        return shareId;
    }
	++shareId;
    m_shareIdDAO.setMaxShareId(shareId);
	std::cout << "Returning new shareId=" << shareId << " " << __FILE__ << " " << __LINE__ << std::endl;
	return shareId;
}

long
ShareIdMgr::readShareId()
{
    return DistribMgr::Instance().getEndShareId();
}



