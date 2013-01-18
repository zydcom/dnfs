#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "cache.h"
#include "list.h"
#include "debug.h"

/*
 * the head of cache list
 */
cache_head cachehead;

/*
 * initialize the cachepool
 */
void cache_init() 
{
	/* initialize the free list and active list */
	INIT_LIST_HEAD(&cachehead.list);
	cachehead.items = 0;
	pthread_mutex_init(&cachehead.lock, NULL);
}

/*
 * find a cache item by pathname in the cache list
 * remove the cache item to the head of list
 */
cache_item* cache_find(const char *path)
{
	cache_item *item;
	pthread_mutex_lock(&cachehead.lock);
	list_for_each_entry(item, &cachehead.list, list) {
		if(!strcmp(item->path, path)) {
			/* remove the target to head */
			if(&item->list != cachehead.list.next)
				cache_move(item); 
			pthread_mutex_unlock(&cachehead.lock);
			/* check if the cache expires */
			int cur_time = time(NULL);
			if((item->readers == 0) && (item->writers == 0)
				 && (cur_time > item->ctime + EXPIRE_TIME)) { 
				cache_del_item(item);
				return NULL;
			}
			return item;
		}
	}
	pthread_mutex_unlock(&cachehead.lock);
	return NULL;
}

/*
 * add a cache item to the head of cache list
 * if total items > CACHE_ITEMS ? remove one from the tail
 */
int cache_add_head(cache_item *item)
{
	pthread_mutex_lock(&cachehead.lock);
	/* cache items reach to limitation*/
	if(cachehead.items == CACHE_ITEMS) {
		/*
		 * if the last item is in use
		 * open will fail
		 */ 
		cache_item *tmp = list_entry(cachehead.list.prev, cache_item, list);
		if(tmp->readers || tmp->writers) {
			pthread_mutex_unlock(&cachehead.lock);
			return -EBUSY;
		}
		else 
			cache_del_tail();
	}
	list_add(&item->list, &cachehead.list);
	cachehead.items++;
	pthread_mutex_unlock(&cachehead.lock);
	return 0;
}

/*
 * remove a cache item from the tail of cache list
 * this function is only executed when lock is acquired!
 */
void cache_del_tail()
{
	/*
	 * temp store the deleted node
	 */
	cache_item *item = list_entry(
		cachehead.list.prev, cache_item, list);
	list_del(cachehead.list.prev);
	cachehead.items--;
	cache_put(item);
}

void cache_del_item(cache_item *item)
{
	pthread_mutex_lock(&cachehead.lock);
	list_del(&item->list);
	cachehead.items--;
	pthread_mutex_unlock(&cachehead.lock);
}

/* 
 * move a cache item to the head of cache list
 * this function is only executed when lock is acquired!
 */
void cache_move(cache_item *item)
{
	list_move(&item->list, &cachehead.list);
}

/*
 * malloc a cache item
 */
cache_item* cache_get()
{
	cache_item *item = (cache_item*)malloc(sizeof(cache_item));
	if(!item) {
		PDEBUG("malloc error in [cache_get]\n");
	}
	/* initialize some members */
	item->ctime = time(NULL);
	item->ino = 0;
	item->wbuf.size = 0;
	item->wbuf.offset = 0;
	item->readers = 0;
	item->writers = 0;
    item->isdirty = 0;
	item->chunks = 0;
	item->wbuf.buf = NULL;
	item->chunklist = NULL;
	item->path = NULL;
	INIT_LIST_HEAD(&item->list);
	
	return item;
}

/*
 * free a cache item
 */
void cache_put(cache_item *item)
{
	if(item->chunklist) {
		free(item->chunklist);
		item->chunklist = NULL;
	}
	if(item->wbuf.buf) {
		free(item->wbuf.buf);
		item->wbuf.buf = NULL;
	}
	if(item->path) {
		free(item->path);
		item->path = NULL;
	}
		
	free(item);
}



