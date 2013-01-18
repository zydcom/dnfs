#ifndef _CACHE_H
#define _CACHE_H

#include <sys/stat.h>
#include <pthread.h>
#include "cs.h"
#include "list.h"

/*
 * the size of cache items
 */
#define CACHE_ITEMS 256      /* 256 files can be opend at the same time */
#define WRITE_BUF 	1048576  /* the size of write buffer (1M Bytes) */
#define EXPIRE_TIME 30       /* cache's expire time (seconds) */

/* 
 * the buffer for write
 * it's space will be freed in flush().
 */
struct write_buffer {
	char *buf;
	unsigned int size;
	unsigned long offset;
};
typedef struct write_buffer write_buffer;

/*
 * struct for cache item 
 */
struct cache_item {
	list_head list;
	uint64_t ctime;         /* the create time of cache item */
	uint64_t ino;           /* the inode number of cached file */
	uint64_t size;          /* the file size of cached file */
	uint16_t readers;       /* how many readers ? */
	uint16_t writers;       /* how many writers ? */ 
    uint16_t isdirty;       /* the data has been changed */
	uint16_t chunks;        /* how many chunks ? */
	chunk_info *chunklist;  /* the chunk list */
	write_buffer wbuf;      /* write buffer information */
	char *path;             /* file pathname */
};
typedef struct cache_item cache_item;

/*
 * cache head with lock protection
 */
struct cache_head {
	list_head list;   
	uint32_t items;        /* how many cache items in the list ? */
	pthread_mutex_t lock;  /* the lock to operate cache list */
};
typedef struct cache_head cache_head;

extern cache_head cachehead;

void cache_init(); 
cache_item* cache_find(const char *path);
int cache_add_head(cache_item *item);
void cache_del_tail();
void cache_del_item(cache_item *item);
void cache_move(cache_item *item);
cache_item* cache_get();
void cache_put(cache_item *item);

#endif