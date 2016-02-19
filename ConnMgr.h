#ifndef _CONNMGR_H_
#define _CONNMGR_H_

#include <map>
#include <ServSocket.h>

#include <Constants.h>

class ConnMgr
{
	ServSocket apps[NO_OF_APPS];
	public:
		ConnMgr();
		~ConnMgr();
		void initializeListeners();
		std::map<int, int> waitAndGetConnections();		

};

#endif
