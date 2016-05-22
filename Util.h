#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>

class Util
{

	public:
		Util();
		~Util();
	        static std::string constructPicFile(int shareId, int appId, const std::string& name);
};

#endif
