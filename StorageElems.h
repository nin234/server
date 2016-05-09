#ifndef _STORAGE_ELEMS_H_
#define _STORAGE_ELEMS_H_

#include <string>
#include <map>
#include <HashMapStr.h>

//create a lock free list/map of template lists and ordinary items

  struct CommonElem
  {       
          HashMapStr archvItems;
          HashMapStr items;
          HashMapStr deviceTokens;
	  LckFreeLstSS lstShareInfo;
	  LckFreeLstSS picShareInfo;
  };

//Each share_id will have an instance of CommonElem
//lstShareInfo is a linked list and each element in the list is a key value
//pair of shareId and list Name issue a shareId can more than one item to share so the key should be shareId:::listName value is listName
//Each item should have an expiry time



#endif
