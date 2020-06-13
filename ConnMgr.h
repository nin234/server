#ifndef _CONNMGR_H_
#define _CONNMGR_H_

#include <map>
#include <ServSocket.h>

#include <Constants.h>
#include <vector>

class ConnMgr
{
	ServSocket apps[NO_OF_APPS];
	ServSocket ssl[NO_OF_APPS];
	void populateConnMp(std::map<AppName, int>& readyFdsMp, int i, int fd);

	public:
		ConnMgr();
		~ConnMgr();
		void initializeListeners();
		std::vector<std::map<AppName, int>> waitAndGetConnections();		

};

#endif
