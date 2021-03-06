#ifndef _NTWINTF_H_
#define _NTWINTF_H_

#include <memory>
#include <Constants.h>
#include <unordered_map>
#include <list>
#include <array>
#include <sys/time.h>
#include <vector>
#include <mutex>
#include <NtwIntfObserver.h>
#include <map>
#include <SSLSocket.h>

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
	std::unordered_map<int, time_t> fdLastActiveMp;
	std::map<int, std::unique_ptr<SSLSocket>> m_fdSSLMp;
	std::mutex fdsQMtx;
	std::vector<int> fdsQ;
	int epfd;
	std::shared_ptr<Decoder> dcd;
	time_t lastCheckTime;
	bool processFreshMessage(char *buffer, ssize_t mlen, int fd);
	bool processFragmentedMessage(char *buffer, ssize_t mlen, ssize_t& remaining, int len,  int fd);
	bool bufferOverFlowCheck(ssize_t remaining, int fd);
	bool processMessage(char *buffer, ssize_t mlen, int fd);
	bool aggrbufinit(int fd);
	void addFdToFdsQ(int fd);
	void addFdsQtoLastActiveMp();
	void updateFdLastActiveMp(int fd);
	void checkAndCleanUpIdleFds();
	bool readMessage(int fd);
	bool readSSLMessage(const std::map<int, std::unique_ptr<SSLSocket>>::iterator& pItr, int fd);
	NtwIntfObserver *m_pObs;

	public:
		NtwIntf();
		~NtwIntf();
		bool waitAndGetMsg();
		char *getNextMsg(int* len);
		bool addFd (int fd);
		bool addSSLFd (int fd);
		void setDecoder(std::shared_ptr<Decoder> pDcd);
		bool sendMsg(char *buf, int mlen, int fd);
		bool sendMsg(char *buf, int mlen, int fd, bool *tryAgain);
		void attach(NtwIntfObserver *pObs);
        void closeAndCleanUpFd(int fd);
};
#endif
