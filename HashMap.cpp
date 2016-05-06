#include <HashMap.h>
#include <stdexcept>
#include <string>

template<typename key, typename val>
HashMap<key, val>::HashMap()
{
	nElems =0;
}

template<typename key, typename val>
HashMap<key, val>::~HashMap()
{

}

template<typename key, typename val>
bool
HashMap<key, val>::getValue(const key& k, val& v)
{
	int indx = hash1(k);
	if (!store[indx].k)
	{
		return false;
	}
	else if (store[indx].k == k)
	{
		v = store[indx].v;
		return true;
	}
	else
	{
		int indx2 = 1;
		indx -= indx2;
		int niter =0;
		while(true)
		{
			++niter;
			if (niter > HASH_MAP_SIZE)
				break;	
			if (indx < 0)
				indx += HASH_MAP_SIZE;	
			if (!store[indx].k)
			{
				return false;
			}
			else if (store[indx].k == k)
			{
				v = store[indx].v;
				return true;
			}	
			else
			{
				int indx2 = 1;
				indx -= indx2;
				if (indx < 0)
					indx += HASH_MAP_SIZE;	
			}
		}
	}	
	return false;
}

template<typename key, typename val>
bool
HashMap<key, val>::find(const key& k)
{
	int indx = hash1(k);
	if (!store[indx].k)
	{
		return false;
	}
	else if (store[indx].k == k)
		return true;
	else
	{
		int indx2 = 1;
		indx -= indx2;
		int niter =0;
		while(true)
		{
			++niter;
			if (niter > HASH_MAP_SIZE)
				break;	
			if (indx < 0)
				indx += HASH_MAP_SIZE;	
			if (!store[indx].k)
			{
				return false;
			}
			else if (store[indx].k == k)
			{
				return true;
			}	
			else
			{
				int indx2 = 1;
				indx -= indx2;
				if (indx < 0)
					indx += HASH_MAP_SIZE;	
			}
		}
	}	
	return false;
}

template<typename key, typename val>
long
HashMap<key, val>::cas(long *reg, long oldval, long newval)
{
    long prev;
    __asm__ __volatile__("lock;"
                 "cmpxchgq %1, %2;"
                 : "=a"(prev)
                 : "r"(newval), "m"(*reg), "0"(oldval)
                 : "memory");
    return prev;
}


template<typename key, typename val>
val&
HashMap<key, val>::operator [] (const key& k)
{
	int indx = hash1(k);
	if (!store[indx].k)
	{
		if (nElems > MAX_ELEMENTS_MAP)
			throw std::overflow_error("Max capacity of HashMap exceeded");
		++nElems;
		store[indx].k = k;
		return store[indx].v;
	}
	else if (store[indx].k == k)
		return store[indx].v;
	else
	{
		if (nElems > MAX_ELEMENTS_MAP)
			throw std::overflow_error("Max capacity of HashMap exceeded");
		++nElems;
		int indx2 = 1;
		indx -= indx2;
		int niter =0;
		while(true)
		{
			++niter;
			if (niter > HASH_MAP_SIZE)
				throw std::overflow_error("Max capacity of HashMap exceeded");
			if (indx < 0)
				indx += HASH_MAP_SIZE;	
			if (!store[indx].k)
			{
				store[indx].k = k;
				cas(&store[indx].k, 0, k);
				if (k == store[indx].k)
					return store[indx].v;
				else
				{
				int indx2 = 1;
				indx -= indx2;
				if (indx < 0)
					indx += HASH_MAP_SIZE;	

				}
			}
			else if (store[indx].k == k)
			{
				return store[indx].v;
			}	
			else
			{
				int indx2 = 1;
				indx -= indx2;
				if (indx < 0)
					indx += HASH_MAP_SIZE;	
			}
		}
	}	
}

template<typename key, typename val>
long
HashMap<key, val>::hash1(key k)
{

	return 0;
}

template<typename key, typename val>
long
HashMap<key, val>::hash2(key k)
{

	return k%HASH_1_PRIME;
}


template class HashMap<long, long>;
template class HashMap<long, std::string>;
template class HashMap<long, CommonElem>;
