#include <ApplePush.h>
#include <stdexcept>
#include <iostream>
#include <stdio.h>

ApplePush::ApplePush(const std::string& file, bool bSbox, std::string passwd): payload(NULL), ctx(NULL)
{
	certfile = file;
	bSandBox = bSbox;

	std::lock_guard<std::mutex> lock(lib_init_mutex);
	if (!lib_init)
	{
		if (apn_library_init() != APN_SUCCESS)
		{
			throw std::runtime_error("Failed to initialize APNS library");
		}
		lib_init = true;
	}
	ctx = apn_init();
	if (ctx == NULL)
	{
		throw std::runtime_error("Failed to initialize APNS library");
	}

	apn_set_pkcs12_file(ctx, certfile.c_str(), passwd.c_str());
	if (bSandBox)
	{
		std::cout << "Setting sandbox mode in apns " << __FILE__ << ":" << __LINE__ << std::endl;
		apn_set_mode(ctx,  APN_MODE_SANDBOX);
	}
	else
	{
		std::cout << "Setting production mode in apns " << __FILE__ << ":" << __LINE__ << std::endl;
		apn_set_mode(ctx,  APN_MODE_PRODUCTION);
	}

	apn_set_behavior(ctx, APN_OPTION_RECONNECT);
	apn_set_log_level(ctx, APN_LOG_LEVEL_INFO | APN_LOG_LEVEL_ERROR | APN_LOG_LEVEL_DEBUG);
    	apn_set_log_callback(ctx, logCallback);
    	apn_set_invalid_token_callback(ctx, invalidTknCallback);
	if(APN_ERROR == apn_connect(ctx))
	{
		throw std::runtime_error("Failed to connect to APNS");
	}

}

std::mutex
ApplePush::lib_init_mutex;

bool
ApplePush::lib_init;


ApplePush::~ApplePush()
{

}



void 
ApplePush::logCallback(apn_log_levels level, const char * const message, uint32_t len)
{
	std::cout <<  message << " " << __FILE__ << ":" << __LINE__ <<  std::endl;
	return;
}


void 
ApplePush::invalidTknCallback(const char * const token, uint32_t index)
{
	std::cout << "invalid token " << token << " index=" << index <<  " " << __FILE__ << ":" << __LINE__ << std::endl; 
	return;
}

bool
ApplePush::send(const std::vector<std::string>& tokenvec, const std::string& msg, int badge)
{
	payload = apn_payload_init();
	if (!payload)
	{
		std::cout << "Unable to init payload " << apn_error_string(errno) << " " << errno << std::endl;
		return false;
	}
        apn_payload_set_badge(payload, badge);
	apn_payload_set_content_available(payload, 1);
        apn_payload_set_body(payload, msg.c_str());
	apn_payload_set_priority(payload, APN_NOTIFICATION_PRIORITY_HIGH);
	
	apn_array_t *tokens = apn_array_init(tokenvec.size(), NULL, NULL);
	if (!tokens)
	{
		apn_payload_free(payload);	
		return false;
	}
	
	for (const std::string& token : tokenvec)
	{
		std::cout << "Adding token apn_array_t " << token << " " << __FILE__ << ":"<< __LINE__  << std::endl;
		apn_array_insert(tokens, const_cast<char*>(token.c_str()));
	}
	apn_array_t *invalid_tokens = NULL;
    if (APN_ERROR == apn_send(ctx, payload, tokens, &invalid_tokens)) 
    {
        std::cout << "Could not send push:" <<  apn_error_string(errno) << " errno:" << errno << std::endl;
	return false;
    } 
     else 
    {
        printf("Notification was successfully sent to %u device(s)\n",
            apn_array_count(tokens) - ((invalid_tokens) ? apn_array_count(invalid_tokens) : 0));
        if (invalid_tokens) {
            printf("Invalid tokens:\n");
            apn_array_free(invalid_tokens);
        }
    }

	apn_payload_free(payload);
       	apn_array_free(tokens);
	return true;
}


