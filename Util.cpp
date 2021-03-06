#include <Util.h>
#include <cstring>
#include <string>
#include <ctime>
#include <sstream>

std::ostream&
operator << (std::ostream& os, const shrIdLstName& shlnm)
{
	os << " shareId=" << shlnm.shareId << " lstName=" << shlnm.lstName << " picLen=" << shlnm.picLen << " appId=" << shlnm.appId << " fd=" << shlnm.fd;
	return os;
}

Util::Util()
{

}

Util::~Util()
{

}

std::string
Util::now()
{
 	char buf[256];
        time_t ctt = time(0);
        struct tm result;
        localtime_r(&ctt, &result);
        asctime_r(&result, buf);
        std::string strnow = buf;
        strnow.pop_back();
	strnow += " ";
	return strnow;
}

std::string
Util::stripSuffix(const std::string& name, const std::string& pattern)
{
	std::size_t posn = name.find_first_of(pattern);
	if (posn == std::string::npos)
		return name;
	std::string strippedStr = name.substr(0, posn);
	return strippedStr;
}

std::string 
Util::getPicRealName(const std::string& name)
{
		char buf[2048];
		strncpy(buf, name.c_str(), 2048);
		char *token = std::strtok(buf,";");
		std::string realName;
		if (token != NULL)
		{
			std::string realName(token);
			return realName;
		}
		std::string tmp;
		return tmp;
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
		char buf[2048];
		strncpy(buf, name.c_str(), 2048);
		char *token = std::strtok(buf,";");
		if (token != NULL)
		{
			filesuffx += token;
			file += filesuffx;
			return file;
		}
		std::string tmp;
		return tmp;
}

std::vector<std::string> 
Util::split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

