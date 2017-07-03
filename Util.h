#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <StorageElems.h>
#include <iostream>

std::ostream& operator << (std::ostream& os, const shrIdLstName& shlnm);

class Util
{

	public:
		Util();
		~Util();
	        static std::string constructPicFile(int shareId, int appId, const std::string& name);
};

#endif
