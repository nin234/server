#include <Config.h>
#include <tinyxml2.h>
#include <iostream>
#include <stdexcept>

Config::Config()
{
	OpenHousesThrds=3;
	AutoSpreeThrds=3;
	EasyGrocThrds=3;
	SmartMsgThrds=3;
	m_bUseDB = true;
	m_bUseRocksDB = true;
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
	pItr = keyvals.find("smartmsg_threads");
	if (pItr != keyvals.end())
	{
		SmartMsgThrds = std::stoi(pItr->second);	
	}
	std::cout <<"Config values OpenHousesThrds=" << OpenHousesThrds << " AutoSpreeThrds=" << AutoSpreeThrds << " EasyGrocThrds=" << EasyGrocThrds  << " SmartMsgThrds=" << SmartMsgThrds << std::endl;
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

	pItr = keyvals.find("apple_sandbox");
	if (pItr != keyvals.end())
	{
		int sb = std::stoi(pItr->second);
		bSandbox = sb?true:false;
	}	
    loadAppConns();    
}

void
Config::loadAppConns()
{

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile("/home/ninan/config/node.xml") != tinyxml2::XML_SUCCESS)
	{
		std::cout << "Failed to parse xml file node.xml " << std::endl;
		throw std::runtime_error("Failed to parse xml file /home/ninan/config/node.xml");
	}
    
    auto apps = doc.FirstChildElement()->FirstChildElement("Apps");

    for (auto app = apps->FirstChildElement(); app != NULL; app = app->NextSiblingElement())
    {
        auto name = app->FirstChildElement("name");
        auto host = app->FirstChildElement("host");
        auto port = app->FirstChildElement("port");
        std::string nameStr = name->GetText();
        std::string hostStr = host->GetText();
        int portVal = std::stoi(port->GetText());
        std::cout << "Adding host details name=" << nameStr << " port=" << portVal << " host=" << hostStr << " " << __FILE__ << ":" << __LINE__ << std::endl;          appConns[nameStr] = std::make_pair(hostStr, portVal); 
    }
    auto shareIds = doc.FirstChildElement()->FirstChildElement("ShareIds");

    auto start = shareIds->FirstChildElement("start");
    m_nShareIdStart = std::stoi(start->GetText());
    auto end = shareIds->FirstChildElement("end");
    m_nShareIdEnd = std::stoi(end->GetText());
}

Config::~Config()
{

}

int
Config::getStartSSLPort()
{
    return m_nStartSSLPort;
}

bool
Config::sandBox()
{
	return bSandbox;
}

bool
Config::useDB()
{
	return m_bUseDB;
}

bool
Config::useRocksDB()
{
	return m_bUseRocksDB;
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

int 
Config::getSmartMsgThrds()
{
	return SmartMsgThrds;
}

int 
Config::getStartShareId()
{
    return m_nShareIdStart;
}

int 
Config::getEndShareId()
{
    return m_nShareIdEnd;
}

std::map<std::string, std::pair<std::string, int>>
Config::getAppConns()
{
    return appConns;
}
