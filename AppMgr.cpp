#include <AppMgr.h>
#include <OpenHousesMgr.h>
#include <AutoSpreeMgr.h>
#include <EasyGrocMgr.h>
#include <SmartMsgMgr.h>
#include <Constants.h>
#include <iostream>
#include <unistd.h>
#include <ShareIdMgr.h>
#include <ApplePush.h>
#include <FirebaseConnHdlr.h>
#include <Config.h>
#include <FrndLstMgr.h>
#include <DistribMgr.h>

AppMgr::AppMgr()
{
    std::cout << "storing distributed node info " << " " << __FILE__ << ":" << __LINE__ << std::endl;   
    DistribMgr::Instance().storeNodeInfo();

	FirebaseConnHdlr::initialize();
	std::cout << "Initialized FirebaseConnHdlr " << " " << __FILE__ << ":" << __LINE__ << std::endl;
	OpenHousesMgr *pOhMgr = new OpenHousesMgr();
	pOhMgr->initialize(Config::Instance().getOpenHousesThrds());
    	wrkMgr[AppName::OPENHOUSES] = pOhMgr;
	std::cout << "Initialized OpenHousesMgr " << " " << __FILE__ << ":" << __LINE__ << std::endl;
	AutoSpreeMgr *pAspreeMgr = new AutoSpreeMgr();
	pAspreeMgr->initialize(Config::Instance().getAutoSpreeThrds());
    	wrkMgr[AppName::AUTOSPREE] = pAspreeMgr;
	std::cout << "Initialized AutoSpreeMgr " << " " << __FILE__ << ":" << __LINE__ << std::endl;
	EasyGrocMgr *pEasy = new EasyGrocMgr();
	pEasy->initialize(Config::Instance().getEasyGrocThrds());
    	wrkMgr[AppName::EASYGROCLIST] = pEasy;
	std::cout << "Initialized EasyGrocMgr " << " " << __FILE__ << ":" << __LINE__ << std::endl;


}

AppMgr::~AppMgr()
{


}

void
AppMgr::setNewSSLClientConnections(const std::map<AppName, int>& appIdFds)
{

	for (const auto& x : appIdFds)
	{
		wrkMgr[x.first]->setNewSSLClientConnection(x.second);
	} 
	return;
}

void
AppMgr::setNewClientConnections(const std::map<AppName, int>& appIdFds)
{

	for (const auto& x : appIdFds)
	{
		wrkMgr[x.first]->setNewClientConnection(x.second);
	} 
	return;
}
