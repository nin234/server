#include <Util.h>

Util::Util()
{

}

Util::~Util()
{

}

std::string 
Util::constructPicFile(int shareId, int appId, const std::string& name)
{
	        std::string file = "/home/ninan/data/pictures/";
		std::string appIdStr = std::to_string(appId);
		file += appIdStr;
		file += "/";
		int dir = shareId%1000;
		std::string dirstr = std::to_string(dir);
		file += dirstr;	
		file += "/";
		std::string filesuffx = std::to_string(shareId);
		filesuffx += "_";
		filesuffx += name;
		file += filesuffx;
		return file;
}
