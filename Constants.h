#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_


#define NO_OF_APPS 3

#define OPENHOUSES_ID 0
#define AUTOSPREE_ID 1
#define EASYGROCLIST_ID 2

#define NO_OF_THREADS 3

#define OPENHOUSES_THREADS 3
#define AUTOSPREE_THREADS 3
#define EASYGROC_THREADS 3

#define OPENHOUSES_MAX_FDS 100
#define AUTOSPREE_MAX_FDS 100
#define EASYGROC_MAX_FDS 1000
#define MAX_FD 100

#define MAX_BUF 1000
#define MAX_EVENTS 5
#define MSG_AGGR_BUF_LEN 8192
#define RCV_BUF_LEN 4096

#define SHARE_ID_START_VAL 1000
#define MAX_MQ_BUF 1000
#define MAX_MQ_EVENTS 5

#define HASH_MAP_SIZE 1600000
#define MAX_ELEMENTS_MAP 1000000
#define HASH_1_PRIME 960259
#define STORE_ARRAY_SIZE 255

#define ARCHIVE_SHARE_ID_MSG 1
#define ARCHIVE_SHARE_TRN_ID_MSG 2
#define ARCHIVE_FRND_LST_MSG 3
#define ARCHIVE_STORE_TEMPL_LST_MSG 4
#define ARCHIVE_LST_MSG 5
#define ARCHIVE_EASYGROC_DEV_TKN_MSG 6
#define ARCHIVE_SHARE_LST_MSG 7

#define BUF_SIZE_32K 32768
#define BUF_SIZE_4K 4096

#define GET_SHARE_ID_MSG 1
#define GET_SHARE_ID_RPLY_MSG 2
#define STORE_TRNSCTN_ID_MSG  3
#define STORE_TRNSCTN_ID_RPLY_MSG  4
#define STORE_FRIEND_LIST_MSG 5
#define STORE_FRIEND_LIST_RPLY_MSG 6
#define NO_COMMON_MSGS 9

#define EASY_GROC_MSG_START 21
#define STORE_TEMPL_LIST_MSG 21
#define STORE_TEMPL_LIST_RPLY_MSG 22
#define SHARE_LIST_MSG 23
#define SHARE_LIST_RPLY_MSG 24
#define STORE_EASYGROC_DEVICE_TKN_MSG 25
#define STORE_EASYGROC_DEVICE_TKN_RPLY_MSG 26
#define GET_EASYGROC_ITEMS 27
#define EASY_GROC_MSG_END 27
#define NO_EASYGROC_MSGS 9

#define GET_EASYGROC_LIST_MSG 40


#define MAX_MSG_ID 23

#define LCKFREE_LST_TDIFF 10.0
#endif
