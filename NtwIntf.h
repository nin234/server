#ifndef _NTWINTF_H_
#define _NTWINTF_H_

#include <memory>
#include <Constants.h>
#include <unordered_map>
#include <list>

struct buf
{
	char *aggrbuf;
	int bufIndx;
};

template<typename Decoder>
class NtwIntf
{
	std::list<char *> pFreeList;
	std::unordered_map<int, buf> pAggrbufs;
	int epfd;
	std::shared_ptr<Decoder> dcd;
	bool processFreshMessage(char *buffer, ssize_t mlen, int fd);
	bool processFragmentedMessage(char *buffer, ssize_t mlen, ssize_t& remaining, int len,  int fd);
	bool bufferOverFlowCheck(ssize_t remaining, int fd);
	bool processMessage(char *buffer, ssize_t mlen, int fd);
	bool aggrbufinit(int fd);	
	public:
		NtwIntf();
		~NtwIntf();
		bool waitAndGetMsg();
		char *getNextMsg(int* len);
		bool addFd (int fd);
		void setDecoder(std::shared_ptr<Decoder> pDcd);
		bool sendMsg(char *buf, int mlen, int fd);
};
#endif
