#include <SSLSocket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


void
SSLSocket::initializeSSL()
{
	SSL_load_error_strings();
    	SSL_library_init();
    	OpenSSL_add_all_algorithms();
}

SSLSocket::SSLSocket()
{

}
