#include <FirebaseConnHdlr.h>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <string>


FirebaseConnHdlr::FirebaseConnHdlr(const std::string& jabberId, const std::string& password)
{
	jid= jabberId;
	pass = password;
}

FirebaseConnHdlr::~FirebaseConnHdlr()
{
}




bool
FirebaseConnHdlr::send(const std::vector<std::string>& tokens, const std::string& msg)
{
    char errbuf[CURL_ERROR_SIZE];
    CURLcode res;
    for (const std::string& token : tokens)
    {
	curl = curl_easy_init(); 
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "https://fcm.googleapis.com/fcm/send");
		curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
		struct curl_slist *list = NULL;
		std::string auth = "Authorization:key=";
		auth += pass;
		list = curl_slist_append(list, "Content-Type:application/json");
		list = curl_slist_append(list, auth.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		std::string data = "{\"to\" : \"";
		data += token;
		data += "\", ";
		data += "\"notification\": {\"title\": \"";
		data += msg;
		data += "\" }, ";
		data += "\"data\": {\"title\": \"";
		data += msg;
		data += "\"} }";
		std::cout << "sending notification to firebase messaging auth=" << auth << " data=" << data << " " << __FILE__ << ":" << __LINE__ << std::endl; 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		res = curl_easy_perform(curl);
		if (res  != CURLE_OK)
		{
			std::cout << "Failed to send http request res=" << res << " " << curl_easy_strerror(res) << " " << __FILE__ << ":" << __LINE__ << std::endl;	
			ssize_t len = strlen(errbuf);
			if (len)
			{
				std::cout << "curl error " << errbuf << " " << __FILE__ << ":" << __LINE__ << std::endl;	
			}
		}
		curl_slist_free_all(list);
		curl_easy_cleanup(curl);

	}


    }    
    return true;
}

void
FirebaseConnHdlr::getSendEvents()
{

	return;
}

bool
FirebaseConnHdlr::connect()
{
	return true;
}

void
FirebaseConnHdlr::initialize()
{
	std::cout << "FirebaseConnHdlr curl initializing" << " " << __FILE__ << ":" << __LINE__ << std::endl;	
	curl_global_init(CURL_GLOBAL_ALL);
	return;
}
