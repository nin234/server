#include <SSLClntSocket.h>
#include <iostream>
#include <Util.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>


thread_local SSL_CTX* SSLClntSocket::pCtx = NULL;

thread_local std::mutex SSLClntSocket::ctx_init;

SSLClntSocket::SSLClntSocket()
{

	if (!pCtx)
	{
		std::lock_guard<std::mutex> ctxLock(ctx_init);
		if (pCtx)
			return;
		std::cout << Util::now() << "Initialising SSL client context " << __FILE__ << " " << __LINE__ << std::endl;
		const SSL_METHOD *method;
		method = TLS_client_method();
		pCtx = SSL_CTX_new(method);
		if (!pCtx)
		{
			std::cout <<Util::now() << "Failed to initialize ssl client context , exiting .... " << std::endl;
			exit(0);
		}


    }
}

SSLClntSocket::~SSLClntSocket()
{

}
