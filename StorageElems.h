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
  };
  

//Template items updated when a user updates (add, modify , delete ) list
//need a lock free data structure that will store list in sort of time order at the same time maintaining a key of names
// [1, 2, 3, 4,     ]
// [WF, BJ, IS, WF]
// 
// []
// time sorted-> [1 2 4 5 8]
// row indx -> 
//
// concurrent tree 
//
//Requirements Template lists updates for user add , modify, delete
//Push notification send to all devices.
//On client reques


#endif
