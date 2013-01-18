#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


#include "server.h"
#include "debug.h"

extern node_head nodehead;
extern file_head *filehead;
extern char meta_root[];
extern uint16_t file_copies;

int nodehead_init()
{
	/* initialize the free list and active list */
	nodehead.items = 0;
	nodehead.offset = 0;
	nodehead.total = 0;
	nodehead.avail = 0;
	nodehead.cur_max = NODE_NUM;
	/* allocate NODE_NUM slots */
	nodehead.list = (node_info*)malloc(NODE_NUM * sizeof(node_info));
	if(!nodehead.list) 
		return -ENOMEM;
	return 0;
}

/* 
 * check if all the nodes work well
 * if not, set hearts to negative value
 */
void* heart_check() 
{
	int i, j;
	while(1)
	{
		PDEBUG("[check node state]\n");
        for(i = 0; i < nodehead.items; i++) {
        	if(nodehead.list[i].hearts >= DOWN_HEARTS) {	
        		/*
        		 * update space information for dnfs
        		 */
        		nodehead.total -= nodehead.list[i].total;
        		nodehead.avail -= nodehead.list[i].avail;
        		/*
                 * remove the node
                 */
        		for(j = i; j < nodehead.items - 1; j++) {
        			nodehead.list[j] = nodehead.list[j+1];
        		}
        		nodehead.items--;
        		if(nodehead.offset == nodehead.items)
        			nodehead.offset = 0;
        		
        		/* print debug message */
        		char ipp[IP_BUF];
        		get_addr(ipp, nodehead.list[i].ip);
        		PDEBUG("[node %s is down]\n", ipp);
        	}
        	else if(nodehead.list[i].hearts >= 0)
        		nodehead.list[i].hearts++;
        	// else the node is down already
        }
        sleep(CHECK_ITVAL);
    }
}

/*
 * operations on file hash list
 */
int filehead_init() 
{
	filehead = (file_head*)malloc(HASH_SLOT * sizeof(file_head));
	if(!filehead)
		return -ENOMEM;
	int i;
	for(i = 0; i < HASH_SLOT; i++) {
		pthread_mutex_init(&filehead[i].lock, NULL);
		filehead[i].items = 0;
		INIT_HLIST_HEAD(&filehead[i].list);
	}
	return 0;
}

file_info* file_get()
{
	file_info *item = (file_info*)malloc(sizeof(file_info));
	if(!item)
		return NULL;
	item->wflag = 0;
	item->path = NULL;
	INIT_HLIST_NODE(&item->list);
	
	return item;
}

void file_put(file_info *item)
{
	if(item->path) {
		free(item->path);
		item->path = NULL;
	}
		
	free(item);
}

unsigned int hash(char* str, unsigned int len)   
{   
   unsigned int hash = 1315423911;   
   unsigned int i    = 0;   
   for(i = 0; i < len; str++, i++)   
   {   
      hash ^= ((hash << 5) + (*str) + (hash >> 2));   
   }   
   
   return hash % HASH_SLOT;   
}
  
file_info* file_find(char *path)
{
	unsigned int hval = hash(path, strlen(path));
	if(!filehead[hval].items)
		return NULL;
	
	file_info *item;
	hlist_node *hnp;
	pthread_mutex_lock(&filehead[hval].lock);
	hlist_for_each_entry(item, hnp, &filehead[hval].list, list) {
		if(!strcmp(item->path, path)) {
			pthread_mutex_unlock(&filehead[hval].lock);
			return item;
		}
	}
	pthread_mutex_unlock(&filehead[hval].lock);
	return NULL;
}

int file_add(file_info *item)
{
	unsigned int hval = hash(item->path, strlen(item->path));
	pthread_mutex_lock(&filehead[hval].lock);
	hlist_add_head(&item->list, &filehead[hval].list);
	filehead[hval].items++;
	pthread_mutex_unlock(&filehead[hval].lock);
	return 0;
}

int file_del(file_info *item)
{
	unsigned int hval = hash(item->path, strlen(item->path));
	pthread_mutex_lock(&filehead[hval].lock);
	hlist_del(&item->list);
	filehead[hval].items--;
	pthread_mutex_unlock(&filehead[hval].lock);
	return 0;
}

void usage()
{
	printf("dnfs-server [-r root-directory] [-c file-copies(1 or 2)]\n");
}

/*
 * parse server command line
 */
int parse_server_cmd(int argc, char *argv[])
{
	int i;
	int ret;
	int srvflag = 0;
	for(i = 1; i < argc; i += 2) {
		if(!strcmp(argv[i], "-c")) {
			int tmp = atoi(argv[i+1]);
			if(tmp != 1 && tmp != 2) {
				usage();
				return -1;
			}
			file_copies = tmp;
		}
		else if(!strcmp(argv[i], "-r")) {
			strcpy(meta_root, argv[i+1]);	
		}
		else {
			usage();
			return -1;
		}
	}
	
	return 0;
}


/* set advisory lock on file */
int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;  /* write lock */
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;  //lock the whole file

    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *filename)
{
    int fd;
    char buf[16];

    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        printf("dnfs-server is already running!");
        return -1;
    }

    /* get file lock */
    if (lockfile(fd) < 0) {
        printf("dnfs-server is already running!");
        return -1;
    }

    /* write pid to lock file */
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return 0;
}

int server_init(int argc, char *argv[])
{
	int ret = 0;
	
	/* signle instance ?? */
	ret = already_running(LOCKFILE);
	IS_CALL_ERR(ret);

	/* parse command line */
	ret = parse_server_cmd(argc, argv);
	IS_CALL_ERR(ret);
	
	/* check if root exsits ? */
	ret = check_directory(meta_root);
	IS_CALL_ERR(ret);
	
	/* initialize node list */
	ret = nodehead_init();
	IS_CALL_ERR(ret);
	
	/* initialize file list */
	ret = filehead_init();
	IS_CALL_ERR(ret);
	
	/* create check heart state thread */
	pthread_t tid;
	if((ret = pthread_create(&tid, NULL, heart_check, NULL)) != 0) {
		perror("pthread_create error in [osd_init]");
		return -1;
	}
	
	/* print initial information */
	printf("metadata root dierctory: %s\n", meta_root);
	printf("file copy number: %d\n", file_copies);
	printf("Metadata server initialized successfully!\n\n");
	
	return 0; 
}

