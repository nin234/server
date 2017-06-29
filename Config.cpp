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
	pItr = keyvals.find("mq_maxmsg");
	if (pItr != keyvals.end())
	{
		mq_maxmsg = std::stoi(pItr->second);
	}
	pItr = keyvals.find("mq_msgsize");
	if (pItr != keyvals.end())
	{
		mq_msgsize = std::stoi(pItr->second);
	}	
	std::cout << "Config values contd mq_maxmsg=" << mq_maxmsg << " mq_msgsize=" << mq_msgsize << std::endl;
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
Config::getMqMaxMsg()
{
	return mq_maxmsg;
}

int 
Config::getMqMsgSize()
{
	return mq_msgsize;
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



