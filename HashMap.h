#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_

#include <array>
#include <Constants.h>
#include <LckFreeLst.h>

template<typename key, typename val>
struct MapElem
{
	key k;
	val v;
	public:
		MapElem() :k{}, v{} {}
		~MapElem(){}
};

template<typename key, typename val, int N=HASH_MAP_SIZE>
class HashMap
{

		long hash1(key k);
		long hash2(key k);
//This hash map uses double hashing described in page 529 of 
//Art of computer programming volume 3 by Don Knuth
//
		std::array<MapElem<key,val>, N> store;
		int nElems;
		long cas(long *reg, long oldval, long newval);

	public:
		HashMap();
		~HashMap();
		bool getValue(const key& k, val& v);
		bool find (const key& k);
		val& operator [](const key& k);
		val& getNext(key& k, bool& isNext, int& indx);
		
};

#endif
