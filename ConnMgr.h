#ifndef _CONNMGR_H_
#define _CONNMGR_H_

#include <map>
#include <ServSocket.h>

#include <Constants.h>

class ConnMgr
{
	ServSocket apps[NO_OF_APPS];
	void populateConnMp(std::map<AppName, int>& readyFdsMp, int i, int fd);

	public:
		ConnMgr();
		~ConnMgr();
		void initializeListeners();
		std::map<AppName, int> waitAndGetConnections();		

};

#endif
