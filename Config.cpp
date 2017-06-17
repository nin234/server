#include <Config.h>
#include <tinyxml2.h>
#include <iostream>
#include <stdexcept>

Config::Config()
{
	OpenHousesThrds=3;
	AutoSpreeThrds=3;
	EasyGrocThrds=3;
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile("/home/ninan/config/config.xml") != tinyxml2::XML_SUCCESS)
	{
		std::cout << "Failed to parse xml file " << std::endl;
		throw std::runtime_error("Failed to parse xml file /home/ninan/config/config.xml");
	}
	for (auto oh = doc.FirstChildElement()->FirstChildElement();oh != NULL; oh = oh->NextSiblingElement())
	{
		keyvals[oh->Name()] = oh->GetText();
		std::cout << "Inserted into keyvals name=" << oh->Name() << " value=" << oh->GetText() << std::endl;
	}
	auto pItr = keyvals.find("openhouses_threads");
	if (pItr != keyvals.end())
	{
		OpenHousesThrds = std::stoi(pItr->second);	
	}	
	pItr = keyvals.find("autospree_threads");
	if (pItr != keyvals.end())
	{
		AutoSpreeThrds = std::stoi(pItr->second);	
	}	
	pItr = keyvals.find("easygroc_threads");
	if (pItr != keyvals.end())
	{
		EasyGrocThrds = std::stoi(pItr->second);	
	}	
	std::cout <<"Config values OpenHousesThrds=" << OpenHousesThrds << " AutoSpreeThrds=" << AutoSpreeThrds << " EasyGrocThrds=" << EasyGrocThrds << std::endl;
}

Config::~Config()
{

}

Config&
Config::Instance()
{
	static Config instance;
	return instance;

}

int
Config::getOpenHousesThrds()
{
	return OpenHousesThrds;
}

int 
Config::getAutoSpreeThrds()
{
	return AutoSpreeThrds;
}

int 
Config::getEasyGrocThrds()
{
	return EasyGrocThrds;
}



