#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <unordered_map>
#include <string>

class Config
{
	int OpenHousesThrds;
	int AutoSpreeThrds;
	int EasyGrocThrds;
	std::unordered_map<std::string, std::string> keyvals;
	public:
		Config();
		virtual ~Config();
		static Config& Instance();
		int getOpenHousesThrds();
		int getAutoSpreeThrds();
		int getEasyGrocThrds();
};
#endif
