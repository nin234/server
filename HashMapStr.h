#ifndef _HASHMAPSTR_H_
#define _HASHMAPSTR_H_

#include <array>
#include <Constants.h>
#include <LckFreeLst.h>
#include <string>
#include<vector>
#include <mutex>
#include <map>

class HashMapStr
{
	static int T[256];

	std::array<LckFreeLstSS, STORE_ARRAY_SIZE> *pHashStore;

		int hash(const char *pStr);
		std::mutex allcmtx;
		std::map<std::string, std::string> storage;
	public:
		HashMapStr();
		~HashMapStr();
		bool insert(const std::string& key, const std::string& val);
		bool getVals(std::vector<std::string>& vals);
		bool getVal(const std::string& key, std::string& val);	
		
		//dummy function to keep compiler happy
		const HashMapStr& operator = (const HashMapStr& rhs){

return *(new HashMapStr());}		
};

#endif
