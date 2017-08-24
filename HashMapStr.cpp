#include <HashMapStr.h>
#include <iostream>


int HashMapStr::T[256] = {
  1,  87,  49,  12, 176, 178, 102, 166, 121, 193,   6,  84, 249, 230,  44, 163,
 14, 197, 213, 181, 161,  85, 218,  80,  64, 239,  24, 226, 236, 142,  38, 200,
110, 177, 104, 103, 141, 253, 255,  50,  77, 101,  81,  18,  45,  96,  31, 222,
 25, 107, 190,  70,  86, 237, 240,  34,  72, 242,  20, 214, 244, 227, 149, 235,
 97, 234,  57,  22,  60, 250,  82, 175, 208,   5, 127, 199, 111,  62, 135, 248,
174, 169, 211,  58,  66, 154, 106, 195, 245, 171,  17, 187, 182, 179,   0, 243,
132,  56, 148,  75, 128, 133, 158, 100, 130, 126,  91,  13, 153, 246, 216, 219, 
119,  68, 223,  78,  83,  88, 201,  99, 122,  11,  92,  32, 136, 114,  52,  10,
138,  30,  48, 183, 156,  35,  61,  26, 143,  74, 251,  94, 129, 162,  63, 152,
170,   7, 115, 167, 241, 206,   3, 150,  55,  59, 151, 220,  90,  53,  23, 131,
125, 173,  15, 238,  79,  95,  89,  16, 105, 137, 225, 224, 217, 160,  37, 123,
118,  73,   2, 157,  46, 116,   9, 145, 134, 228, 207, 212, 202, 215,  69, 229,
 27, 188,  67, 124, 168, 252,  42,   4,  29, 108,  21, 247,  19, 205,  39, 203,
233,  40, 186, 147, 198, 192, 155,  33, 164, 191,  98, 204, 165, 180, 117,  76,
140,  36, 210, 172,  41,  54, 159,   8, 185, 232, 113, 196, 231,  47, 146, 120,
 51,  65,  28, 144, 254, 221,  93, 189, 194, 139, 112,  43,  71, 109, 184, 209
};

HashMapStr::HashMapStr():pHashStore(NULL)
{

}

HashMapStr::~HashMapStr()
{
	/*
	if (pHashStore)
		delete pHashStore;
	*/
}

int
HashMapStr::hash(const char *pStr)
{
	unsigned char hash=0;
	for (int i=0; pStr[i] != '\0'; ++i)
	{
		hash = T[hash ^ pStr[i]];
	}
	return hash;
}

bool
HashMapStr::insert(const std::string& key, const std::string& val)
{
	std::lock_guard<std::mutex> lock(allcmtx);
	storage[key] = val;
	/*
	int hsh = hash(key.c_str());
	hsh %= STORE_ARRAY_SIZE;
	if (!pHashStore)
	{
		std::lock_guard<std::mutex> lock(allcmtx);
		if (!pHashStore)
			 pHashStore = new std::array<LckFreeLstSS, STORE_ARRAY_SIZE>;
	}
	std::cout << "Inserting to HashMapStr hsh=" << hsh << " key=" << key << " val=" << val << " " << __FILE__ << ":" << __LINE__ << std::endl;
	(*pHashStore)[hsh].insert(key, val);
	*/
	return true;
}

bool
HashMapStr::getVal(const std::string& key, std::string& val)
{
	std::lock_guard<std::mutex> lock(allcmtx);
	auto pItr = storage.find(key);
	if (pItr == storage.end())
		return false;
	val = pItr->second;
	return true;

/*
	int hsh = hash(key.c_str());
	hsh %= STORE_ARRAY_SIZE;
	if (!pHashStore)
	{
		std::cout << "Failed to find hash store for key=" << key << " " << __FILE__ << ":" << __LINE__ << std::endl;
		return false;
	}
	std::cout << "Retrieving from HashMapStr hsh=" << hsh << " key=" << key << " val=" << val << " " << __FILE__ << ":" << __LINE__ << std::endl;
	return (*pHashStore)[hsh].getVal(key, val);	
*/
}

bool
HashMapStr::getVals(std::vector<std::string>& vals)
{
	std::lock_guard<std::mutex> lock(allcmtx);
	for (auto pItr = storage.begin(); pItr != storage.end(); ++pItr)
		vals.push_back(pItr->second);
	return true;

/*
	if (!pHashStore)
		return false;
	for (const LckFreeLstSS& lst : *pHashStore)
		lst.getVals(vals);	
	return true;
*/
}


