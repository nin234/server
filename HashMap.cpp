#include <HashMap.h>
#include <stdexcept>
#include <string>
#include <StorageElems.h>

template<typename key, typename val, int N>
HashMap<key, val, N>::HashMap()
{
	nElems =0;
}

template<typename key, typename val, int N>
HashMap<key, val, N>::~HashMap()
{

}

template<typename key, typename val, int N>
bool
HashMap<key, val, N>::getValue(const key& k, val& v)
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
			if (niter > N)
				break;	
			if (indx < 0)
				indx += N;	
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
					indx += N;	
			}
		}
	}	
	return false;
}

template<typename key, typename val, int N>
bool
HashMap<key, val, N>::find(const key& k)
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
			if (niter > N)
				break;	
			if (indx < 0)
				indx += N;	
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
					indx += N;	
			}
		}
	}	
	return false;
}

template<typename key, typename val, int N>
long
HashMap<key, val, N>::cas(long *reg, long oldval, long newval)
{
    long prev;
    __asm__ __volatile__("lock;"
                 "cmpxchgq %1, %2;"
                 : "=a"(prev)
                 : "r"(newval), "m"(*reg), "0"(oldval)
                 : "memory");
    return prev;
}


template<typename key, typename val, int N>
val&
HashMap<key, val, N>::operator [] (const key& k)
{
	int indx = hash1(k);
	if (!store[indx].k)
	{
		if (nElems > N)
			throw std::overflow_error("Max capacity of HashMap exceeded");
		++nElems;
		store[indx].k = k;
		return store[indx].v;
	}
	else if (store[indx].k == k)
		return store[indx].v;
	else
	{
		if (nElems > N)
			throw std::overflow_error("Max capacity of HashMap exceeded");
		++nElems;
		int indx2 = 1;
		indx -= indx2;
		int niter =0;
		while(true)
		{
			++niter;
			if (niter > N)
				throw std::overflow_error("Max capacity of HashMap exceeded");
			if (indx < 0)
				indx += N;	
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
					indx += N;	

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
					indx += N;	
			}
		}
	}	
}

template<typename key, typename val, int N>
long
HashMap<key, val, N>::hash1(key k)
{

	return k%N;
}

template<typename key, typename val, int N>
long
HashMap<key, val, N>::hash2(key k)
{

	return k%N;
}

template<typename key, typename val, int N>
bool
HashMap<key, val, N>::getNext(key& k, val& v, int &indx)
{
	for (int i=indx+1; i < N; ++i)
	{
		if (!store[i].k)
			continue;
		k = store[i].k;
		v = store[i].v;
		return true;
	}
	return false;
}


template class HashMap<long, long>;
template class HashMap<long, std::string>;
template class HashMap<long, CommonElem>;
template class HashMap<long, LckFreeLstSS, 10>;
template class HashMap<long, LckFreeLstSL, 10>;
