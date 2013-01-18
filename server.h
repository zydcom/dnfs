#ifndef _CONFIG_H
#define _CONFIG_H

#include <pthread.h>
#include "list.h"

/* 
 * the config file.
 */
#define NODE_NUM     32    /* first alloc size for node management */
#define IP_BUF       16    /* buffer for storing ip string */
#define LINE_BUF	 128   /* line buffer for configuration */
//#define PATH_BUF     256   /* buffer for storing path name */

/*
 * default config value
 */
#define META_ROOT   	 "/mnt/meta" /* default root directory */
#define DNFS_BLK         1024 /* dnfs block size */
#define SPACE_RESERVE    100  /* the space(MB) a storage node should reserve */
#define FILE_COPIES      2    /* default copy number the system keeps for a new file */
#define CHECK_ITVAL      5    /* interval for checking storage nodes' heart report */
#define DOWN_HEARTS      4    /* the counter to denote a node down */
#define HASH_SLOT        256  /* the hash list's slot number */

/* single server instance */
#define LOCKFILE "/var/run/dnfs-server.pid"

struct node_info {
	uint32_t ip;
	uint32_t total;
	uint32_t avail;
	int hearts;  /* hearts less then zero denotes a broken node */
};
typedef struct node_info node_info;

/* 
 * node information 
 */
struct node_head {
	node_info *list;
	uint16_t items;
	uint16_t offset;
	uint16_t cur_max;
	uint64_t total;  /* total space in KB */
	uint64_t avail;  /* available space in KB */
};
typedef struct node_head node_head;

/* 
 * file information
 */
struct file_info {
	hlist_node list;
	int wflag;
	char *path;
};
typedef struct file_info file_info;

struct file_head {
	int items;
	hlist_head list;
	pthread_mutex_t lock;
};
typedef struct file_head file_head;


/*
 * operations on file hash list
 */
int filehead_init();
file_info* file_get();
void file_put(file_info *item);
file_info* file_find(char *path);
int file_add(file_info *item);
int file_del(file_info *item);


#endif
