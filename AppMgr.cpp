#include <AppMgr.h>
#include <OpenHousesMgr.h>
#include <AutoSpreeMgr.h>
#include <EasyGrocMgr.h>
#include <Constants.h>
#include <iostream>
#include <unistd.h>
#include <ShareIdMgr.h>
#include <ApplePush.h>
#include <FirebaseConnHdlr.h>
#include <Config.h>

AppMgr::AppMgr()
{
	FirebaseConnHdlr::initialize();
	std::cout << "Initialized FirebaseConnHdlr " << " " << __FILE__ << ":" << __LINE__ << std::endl;
	OpenHousesMgr *pOhMgr = new OpenHousesMgr();
	pOhMgr->initialize(Config::Instance().getOpenHousesThrds());
    wrkrMgr[AppName::OPENHOUSES] = pOhMgr;
	std::cout << "Initialized OpenHousesMgr " << " " << __FILE__ << ":" << __LINE__ << std::endl;
	AutoSpreeMgr *pAspreeMgr = new AutoSpreeMgr();
	pAspreeMgr->initialize(Config::Instance().getAutoSpreeThrds());
    wrkrMgr[AppName::AUTOSPREE] = pAspreeMgr;
	std::cout << "Initialized AutoSpreeMgr " << " " << __FILE__ << ":" << __LINE__ << std::endl;
	EasyGrocMgr *pEasy = new EasyGrocMgr();
	pEasy->initialize(Config::Instance().getEasyGrocThrds());
    wrkrMgr[AppName::EASYGROCLIST] = pEasy;
	std::cout << "Initialized EasyGrocMgr " << " " << __FILE__ << ":" << __LINE__ << std::endl;
	ShareIdMgr::Instance().setShareIdStep(Config::Instance().getOpenHousesThrds() + Config::Instance().getAutoSpreeThrds() + Config::Instance().getEasyGrocThrds());

}

AppMgr::~AppMgr()
{


}


void
AppMgr::setNewClientConnections(const std::map<AppName, int>& appIdFds)
{

	for (const auto& x : appIdFds)
	{
		if (x.first >= NO_OF_APPS)
		{
			std::cout << "Invalid app number " << x.first << " " << __FILE__ << " " << __LINE__ << std::endl;
			close(x.second);
			continue;
		}
		wrkMgr[x.first].setNewClientConnection(x.second);
	} 
	return;
}
