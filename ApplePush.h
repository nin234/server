#ifndef _APPLE_PUSH_H_
#define _APPLE_PUSH_H_

#include <capn/apn.h>
#include <mutex>
#include <string>
#include <vector>


class ApplePush
{
		apn_payload_t *payload;
    		apn_ctx_t *ctx;
		static std::mutex lib_init_mutex;
		static bool lib_init;
		std::string certfile;
		bool bSandBox;

	public:
		ApplePush(const std::string& file , bool bSbox);
		~ApplePush();
		static void logCallback(apn_log_levels level, const char * const message, uint32_t len);
		static void invalidTknCallback(const char * const token, uint32_t index);
		bool send(const std::vector<std::string>& tokens, const std::string& msg, int badge);
};

#endif
