#include <AppMgr.h>
#include <OpenHousesMgr.h>
#include <AutoSpreeMgr.h>
#include <EasyGrocMgr.h>
#include <Constants.h>
#include <iostream>
#include <unistd.h>
#include <ShareIdMgr.h>
#include <ApplePush.h>

AppMgr::AppMgr()
{
	OpenHousesMgr *pOhMgr = new OpenHousesMgr();
	pOhMgr->initialize(OPENHOUSES_THREADS);
	wrkMgr.push_back(pOhMgr);
	AutoSpreeMgr *pAspreeMgr = new AutoSpreeMgr();
	pAspreeMgr->initialize(AUTOSPREE_THREADS);
	wrkMgr.push_back(pAspreeMgr);
	EasyGrocMgr *pEasy = new EasyGrocMgr();
	pEasy->initialize(EASYGROC_THREADS);
	wrkMgr.push_back(pEasy);
	ShareIdMgr::Instance().setShareIdStep(OPENHOUSES_THREADS+AUTOSPREE_THREADS+EASYGROC_THREADS);

}

AppMgr::~AppMgr()
{


}


void
AppMgr::setNewClientConnections(const std::map<int, int>& appIdFds)
{

	for (const auto& x : appIdFds)
	{
		if (x.first >= NO_OF_APPS)
		{
			std::cout << "Invalid app number " << x.first << " " << __FILE__ << " " << __LINE__ << std::endl;
			close(x.second);
			continue;
		}
		wrkMgr[x.first]->setNewClientConnection(x.second);
	} 
	return;
}
