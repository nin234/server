#ifndef _LCKFREELST_H_
#define _LCKFREELST_H_

#include <map>
#include <mutex>
#include <time.h>
#include <string>
#include <vector>

template<typename KeyType, typename ValType>
struct Node
{
	KeyType key;
	ValType val;
	Node *next;
	bool marked;
	Node();
	Node (KeyType key, ValType val);
	~Node();
	bool is_marked_reference() {return marked;}
};

template<typename KeyType, typename ValType>
class LckFreeLst
{

	Node<KeyType, ValType> *head;
	Node<KeyType, ValType> *tail;
	Node<KeyType, ValType> *search(KeyType search_key, Node<KeyType, ValType> **left_node);
	std::map<time_t, Node<KeyType, ValType>*> garbageCollector;
	std::mutex gcmutex;


	std::map<KeyType, ValType> storage;

	public:
		LckFreeLst();
		~LckFreeLst();
		bool insert(KeyType key, ValType val);
		bool insertOrUpdate(KeyType key, ValType val);
		bool is_marked_reference(Node<KeyType, ValType> *pNode);
		bool find (KeyType key);
		bool erase (KeyType key);
		bool erasepmatch (KeyType key);
		void cleanUp();
		void getVals(std::vector<ValType>& vals);
		void getKeys(std::vector<KeyType>& keys);
		void getKeyVals(std::map<KeyType, ValType>& kvals);
		bool getVal(const KeyType& key, ValType& val);
		//dummy fn to make compiler happy
		const LckFreeLst& operator=(const LckFreeLst& rhs)
		{ return *(new LckFreeLst());}
		bool isEmpty();
};

struct timstr
{
	std::string str;
	long tv_sec;
	long tv_usec;
};
typedef LckFreeLst<std::string, std::string> LckFreeLstSS;
typedef LckFreeLst<std::string, long> LckFreeLstSL;
typedef LckFreeLst<std::string, timstr> LckFreeLstSTS;

#endif
