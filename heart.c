#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/statvfs.h>
#include <pthread.h>
#include <unistd.h>

#include "cs.h"
#include "common.h"
#include "storage.h"
#include "debug.h"

/*
 * default value
 * can be configured
 */
char meta_server[IP_BUF];
int  heart_itval = HEART_ITVAL;
char storage_root[PATH_BUF] = STORAGE_ROOT;

/*
 * total and available space (KB) in storage resided fs
 */
int get_space_info(uint32_t *total, uint32_t *avail)
{
	struct statvfs stbuf;
	if (statvfs(storage_root, &stbuf)) {
		perror("statfs error in [get_avail_space]");
		return -errno;
	}
	
	*total = stbuf.f_blocks * (stbuf.f_bsize / 1024);
	*avail = stbuf.f_bavail * (stbuf.f_bsize / 1024);

	return 0; 
}


/* 
 * call os_heart with udp protocol
 */
int rpccall_os_heart(os_heart_in *arg, os_heart_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(meta_server, OS_PROG, OS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(os_heart_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call rpc os_heart_1 error."));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_os_heart(os_heart_in *arg, os_heart_out *res, char *ipp)
{
	int ret = rpccall_os_heart(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

void *heart_state()
{
	uint32_t space;
	os_heart_in arg;
	os_heart_out res;
	int ret;
	
	while(1) {
		ret = get_space_info(&arg.total, &arg.avail);
		PTR_ERR(ret);

		PDEBUG("[hear report]: total=%uKB, avail = %uKB\n", arg.total, arg.avail);
		
		rpc_os_heart(&arg, &res, meta_server);
		sleep(heart_itval);
	}
}

void usage()
{
	printf("dnfs-node -s server-ip [-r root-directory] [-i heart-interval]\n");
}

/*
 * parse storage node's command line
 */
int parse_storage_cmd(char argc, char *argv[])
{
	int i;
	int ret;
	int srvflag = 0;
	for(i = 1; i < argc; i += 2) {
		if(!strcmp(argv[i], "-s")) {
			if(!is_ipv4(argv[i+1])) {
				printf("invaild ip address.\n");
				return -1;
			}
			strcpy(meta_server, argv[i+1]);
			srvflag = 1;
		}
		else if(!strcmp(argv[i], "-r")) {
			ret = check_directory(argv[i+1]);
			IS_CALL_ERR(ret);
			strcpy(storage_root, argv[i+1]);
		}
		else if(!strcmp(argv[i], "-i")) {
			heart_itval = atoi(argv[i+1]);
			
		}
		else {
			usage();
			return -1;
		}
	}
	
	if(!srvflag) {
		usage(); 
		return -1;
	}
	
	return 0;
}


int storage_init(int argc, char *argv[])
{
	int ret;
	
	/* parse command line */
	ret = parse_storage_cmd(argc, argv);
	IS_CALL_ERR(ret);
	
	/* check if root exsits? */
	ret = check_directory(storage_root);
	IS_CALL_ERR(ret);
	
	/* create send heart state thread */
	pthread_t tid;
	if((ret = pthread_create(&tid, NULL, heart_state, NULL)) != 0) {
		perror("pthread_create error in [osd_init]");
		return -1;
	}
	
	/* print initial information */
	printf("metadata server: %s\n", meta_server);
	printf("storage root dierctory: %s\n", storage_root);
	printf("heart interval: %d seconds\n", heart_itval);
	printf("Storage node initialized successfully!\n\n"); 
	
	return 0;
}


