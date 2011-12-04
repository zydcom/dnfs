#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <malloc.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <shadow.h>

#include "cs.h"
#include "wrapper.h"
#include "debug.h"
#include "common.h"
#include "server.h"

/*
 * all the server function keeps the last error value
 * and put it in res->err
 */
 
/* 
 * default value
 * can be configured.
 */
char meta_root[PATH_BUF] = META_ROOT;
uint16_t file_copies = FILE_COPIES;
node_head nodehead;
file_head *filehead;

/* 
 * convert a pathname, add prefix meta_root 
 */
void get_meta_path(char *dest, char *src)
{
	sprintf(dest, "%s%s", meta_root, src);
}

/*
 * read dnfs_stat struct from meta file
 * this method is position independent
 */
void meta_read_dnfs_stat(dnfs_stat *st, FILE *fp) 
{
	fseek(fp, 0, SEEK_SET);
	fread(st, sizeof(dnfs_stat), 1, fp);
}

/*
 * write dnfs_stat struct into meta file
 * this method is position indepent
 */
void meta_write_dnfs_stat(dnfs_stat *st, FILE *fp)
{
	fseek(fp, 0, SEEK_SET);
	fwrite(st, sizeof(dnfs_stat), 1, fp);
}

/*
 * add an chunk information to meta file
 * position independent
 */
void meta_add_chunk_info(chunk_info *chunk, dnfs_stat *st, FILE *fp)
{
	int i;
	int offset = sizeof(dnfs_stat) + st->s_copies * st->s_chunks * sizeof(uint32_t);
	fseek(fp, offset, SEEK_SET);
	fwrite(&chunk->ip, sizeof(uint32_t), 1, fp);
	if(st->s_copies == 2)
		fwrite(&chunk->bip, sizeof(uint32_t), 1, fp);
	st->s_chunks++;
}

/*
 * read chunk information from meta file
 * position indepent
 */
void meta_read_chunk_info(chunk_info *chunks, dnfs_stat *st, FILE *fp)
{
	int i;
	for(i = 0; i < st->s_chunks; i++) {
		fread(&chunks[i].ip, sizeof(uint32_t), 1, fp);
		if(st->s_copies == 2)
			fread(&chunks[i].bip, sizeof(uint32_t), 1, fp);
		else 
			chunks[i].bip = 0;
	}
}

/*
 * get the ip address(uint32_t) of the caller
 */
uint32_t get_caller(struct svc_req *rqst)
{
	struct sockaddr_in *si = svc_getcaller(rqst->rq_xprt);
	char *ipp = inet_ntoa(si->sin_addr);
	uint32_t ip;
	put_addr(ipp, &ip);
	return ip;
}

/*
 * implemention of cs_getattr function
 * lstat result for directory
 * meta file information for regular file
 */
bool_t cs_getattr_1_svc(cs_getattr_in * arg, cs_getattr_out * res, struct svc_req * rqst)
{
	PDEBUG("[cs_getattr_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	struct stat st;
	res->err = wrapper_lstat(path, &st);
	IS_ERR(res->err);
	
	if(S_ISREG(st.st_mode)) {
		FILE *fp = fopen(path, "r");
		if(!fp) {
			res->err = -errno;
			return TRUE;
		}
		meta_read_dnfs_stat(&res->st, fp);
		fclose(fp);
	}
	else if(S_ISDIR(st.st_mode)) {
		set_dnfs_attr(st, &res->st);
		res->st.s_chunks = 0;
		res->st.s_copies = 0;
	}
	
	return TRUE;
}

/*
 * implemention of cs_access function
 * call access system call to check permission
 */
bool_t cs_access_1_svc(cs_access_in *arg, cs_access_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_access_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	res->err = wrapper_access(path, arg->mode);
	return TRUE;
}

/*
 * implemention of cs_mkdir function
 * just create the directory in meta_root
 */
bool_t cs_mkdir_1_svc(cs_mkdir_in *arg, cs_mkdir_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_mkdir_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	res->err = wrapper_mkdir(path, arg->mode);
	return TRUE;
}

/*
 * implemention of cs_rmdir function
 * just delete the directory in meta_root
 */
bool_t cs_rmdir_1_svc(cs_rmdir_in *arg, cs_rmdir_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_rmdir_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	res->err = wrapper_rmdir(path);
	return TRUE;
}

/*
 * implemention of cs_readdir function
 * read 30 items a time for 8192bytes limitation
 */
bool_t cs_readdir_1_svc(cs_readdir_in *arg, cs_readdir_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_readdir_1_svc]: %s\n", arg->path);	
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	DIR *dp;
	struct dirent *dir;
	
	/* the first call */
	if(!arg->dp) { 
		res->err = wrapper_opendir(path, &dp);
		IS_ERR(res->err);
	}
	else 
		dp = (DIR*)(arg->dp);

	res->dirs.dirs_val = (dnfs_dir*)malloc(DIR_BUF * sizeof(dnfs_dir));
	if(!res->dirs.dirs_val) {
		res->err = -ENOMEM;
		PDEBUG(strerror(ENOMEM));
		return TRUE;
	}	
	
	int cnt = 0;
    while(cnt < DIR_BUF) {
    	res->err = wrapper_readdir(dp, &dir);
    	IS_ERR(res->err);
    	if(!dir) {
    		res->err = 1; /* finish flag */	
    		break;
    	}
    	/* fill dnfs_dir struct */
    	strcpy(res->dirs.dirs_val[cnt].d_name, dir->d_name);
    	res->dirs.dirs_val[cnt].d_type = dir->d_type;
    	res->dirs.dirs_val[cnt].d_ino = dir->d_ino;
    	cnt++;
    }
	res->dirs.dirs_len = cnt;
	res->dp = (unsigned long)dp;
	
	/* close the opened directory */
	if(res->err == 1)
		wrapper_closedir(dp);

	return TRUE;
}

/*
 * implemention of cs_mknod function
 * just create a new file and write meta information
 */
bool_t cs_mknod_1_svc(cs_mknod_in *arg, cs_mknod_out *res, struct svc_req *rqst)
{	
	PDEBUG("[cs_mknod_1_svc]: %s\n", arg->path);	
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	mode_t mode = arg->mode;
	
	/* just support regular file */
	if(!S_ISREG(mode)) {
		res->err = -ENOSYS;
		return TRUE;
	}
	
	/* 
	 * initialize dnfs_stat struct for the file 
	 * write it into the meta file
	 */ 

	FILE *fp;
	dnfs_stat st;
	struct stat stbuf;

	fp = fopen(path, "w+");
	if(!fp) {
		res->err = -errno;
		return TRUE;
	}
	
	res->err = wrapper_lstat(path, &stbuf);
	IS_ERR(res->err);
	
	set_dnfs_attr(stbuf, &st);
	
	/* set chunks as 0 and set default file copy value */
	st.s_chunks = 0;
	st.s_copies = file_copies;
	
	meta_write_dnfs_stat(&st, fp);
	fclose(fp);
	
	return TRUE;
}

/*
 * implemention of cs_open function
 * transfer all the meta data to client
 */
bool_t cs_setwflag_1_svc(cs_setwflag_in *arg, cs_setwflag_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_setwflag_1_svc]: %s\n", arg->path);	

	int ret = 0;
	res->err = 0;
	
	file_info *item = file_find(arg->path);
	if(!item) {
		item = file_get();
		item->path = (char*)malloc(strlen(arg->path) + 1);
		strcpy(item->path, arg->path);
		file_add(item);
	}
	
	if(arg->promote == 1) {
		if(item->wflag) {
			res->err = -EBUSY;
			return TRUE;
		}
		item->wflag = 1;
	}
	else if(arg->promote == -1)
		item->wflag = 0;
	
	if(!item->wflag) {
		file_del(item);
		file_put(item);
	}
	
	return TRUE;
}


/*
 * implemention of cs_open function
 * transfer all the meta data to client
 */
bool_t cs_open_1_svc(cs_open_in *arg, cs_open_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_open_1_svc]: %s\n", arg->path);	
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;

	/* check read or write permission when fetching metadata */
	file_info *item = file_find(arg->path);
	if(item && item->wflag) {
		res->chunks.chunks_len = 0;
		res->chunks.chunks_val = NULL;
		res->ino = 0;
		res->size = 0;
		res->err = -EBUSY;
		return TRUE;
	}
	
	/* ok! you can get metadata */
	res->chunks.chunks_len = 0;
	res->chunks.chunks_val = (chunk_info*)malloc(CHUNK_BUF * sizeof(chunk_info));
	if(!res->chunks.chunks_val) {
		res->err = -ENOMEM;
		PDEBUG(strerror(ENOMEM));
		return TRUE;
	}

	FILE *fp;
	dnfs_stat st;
	
	fp = fopen(path, "r+");
	if(!fp) {
		res->err = -errno;
		return TRUE;
	}
	meta_read_dnfs_stat(&st, fp);
	res->ino = st.s_ino;
	res->size = st.s_size;
	
	/* update atime */
	st.s_atime = time(NULL);

	meta_write_dnfs_stat(&st, fp);
	meta_read_chunk_info(res->chunks.chunks_val, &st, fp);
	res->chunks.chunks_len = st.s_chunks;
	fclose(fp);
	
	return TRUE;
}

/* 
 * node must have space more than SPACE_RESERVE
 * maybe there is no node available, return 0, then
 * using Round Robin Algorithm
 */
uint32_t select_chunk_rr()
{
	int loop = 0;
	int result;
	if(!nodehead.items)
		return 0;
	while(nodehead.list[nodehead.offset].avail / 1024 < SPACE_RESERVE) {
		nodehead.offset = (nodehead.offset + 1) % nodehead.items;
		loop++;
		/* no node available */
		if(loop == nodehead.items) {
			return 0;
	    }
	}
	result = nodehead.list[nodehead.offset].ip;
	nodehead.offset = (nodehead.offset + 1) % nodehead.items;
	return result;
}

/*
 * implemention of cs_newchunk function
 * the storage list is updated in this function 
 * so release can do less work
 */
bool_t cs_newchunk_1_svc(cs_newchunk_in *arg, cs_newchunk_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_newchunk_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	FILE *fp;
	dnfs_stat st;
	
	fp = fopen(path, "r+");
	if(!fp) {
		res->err = -errno;
		return TRUE;
	}

	meta_read_dnfs_stat(&st, fp);
	
	res->chunk.ip = select_chunk_rr();
	if(!res->chunk.ip)
		return TRUE;
	res->chunk.bip = 0;
	
	if(st.s_copies == 2) {
		res->chunk.bip = select_chunk_rr();
		/* only one available node */
		if(res->chunk.ip == res->chunk.bip)
			res->chunk.bip = 0;
	}
	
	/*
	 * add the new storage chunk info to meta file
	 */
	meta_add_chunk_info(&res->chunk, &st, fp);
    meta_write_dnfs_stat(&st, fp);
	fclose(fp);

	return TRUE;
}

/*
 * update meta data after a write operation
 * only called when client has done write operation
 */
bool_t cs_release_1_svc(cs_release_in *arg, cs_release_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_release_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	FILE *fp;
	dnfs_stat st;
	
	fp = fopen(path, "r+");
	if(!fp) {
		res->err = -errno;
		return TRUE;
	}
	
	meta_read_dnfs_stat(&st, fp);
	
	/* update size and mtime member */
	st.s_size = arg->size;
	st.s_mtime = time(NULL);
	
	meta_write_dnfs_stat(&st, fp);
	fclose(fp);

	return TRUE;
}


/*
 * implemention of cs_getlist function
 * transfer all the meta data to client
 */
bool_t cs_getlist_1_svc(cs_getlist_in *arg, cs_getlist_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_getlist_1_svc]: %s\n", arg->path);	
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	res->chunks.chunks_val = (chunk_info*)malloc(CHUNK_BUF * sizeof(chunk_info));
	if(!res->chunks.chunks_val) {
		res->err = -ENOMEM;
		PDEBUG(strerror(ENOMEM));
		return TRUE;
	}

	FILE *fp;
	dnfs_stat st;
	
	fp = fopen(path, "r+");
	if(!fp) {
		res->err = -errno;
		return TRUE;
	}

	meta_read_dnfs_stat(&st, fp);
	res->ino = st.s_ino;
	meta_write_dnfs_stat(&st, fp);
	meta_read_chunk_info(res->chunks.chunks_val, &st, fp);
	res->chunks.chunks_len = st.s_chunks;
	fclose(fp);
	
	return TRUE;
}

/*
 * unlink meta file
 */
bool_t cs_unlink_1_svc(cs_unlink_in *arg, cs_unlink_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_unlink_1_svc]: %s\n", arg->path);	
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	res->err = wrapper_unlink(path);
	return TRUE;
}

/*
 * truncate a file to zero
 */
bool_t cs_truncate_1_svc(cs_truncate_in *arg, cs_truncate_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_truncate_1_svc]: %s, %llu\n", arg->path, arg->size);	
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	FILE *fp;
	dnfs_stat st;
	
	fp = fopen(path, "r+");
	if(!fp) {
		res->err = -errno;
		return TRUE;
	}
	
	meta_read_dnfs_stat(&st, fp);
	
	/* update size and chunks member */
	st.s_size = 0;
	st.s_chunks = 0;
	
	meta_write_dnfs_stat(&st, fp);
	fclose(fp);
	
	/* truncate the meta file */ 
	res->err = wrapper_truncate(path, sizeof(dnfs_stat));

	return TRUE;
}

/*
 * change the uid and gid of meta file
 * for regular file, change meta information
 * for directory, just call wrapper_chown
 */
bool_t cs_chown_1_svc(cs_chown_in *arg, cs_chown_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_chown_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	struct stat stbuf;
	res->err = wrapper_lstat(path, &stbuf);
	IS_ERR(res->err);
	
	if(S_ISREG(stbuf.st_mode)) {
    	FILE *fp;
    	dnfs_stat st;
    	
    	fp = fopen(path, "r+");
    	if(!fp) {
			res->err = -errno;
			return TRUE;
		}
    
    	meta_read_dnfs_stat(&st, fp);

        /* update uid, gid */
    	st.s_uid = arg->owner;
    	st.s_gid = arg->group;
    	st.s_ctime = time(NULL);
    	
    	meta_write_dnfs_stat(&st, fp);
    	fclose(fp);
	}
	else if(S_ISDIR(stbuf.st_mode)) {
		res->err = wrapper_chown(path, arg->owner, arg->group);
		IS_ERR(res->err);
	}
	
	return TRUE;
}

/*
 * change the uid and gid of meta file
 * for regular file, change meta information
 * for directory, just call wrapper_chmod
 */
bool_t cs_chmod_1_svc(cs_chmod_in *arg, cs_chmod_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_chmod_1_svc]: %s\n", arg->path);
	
	char path[PATH_BUF];
	get_meta_path(path, arg->path);
	res->err = 0;
	
	struct stat stbuf;
	res->err = wrapper_lstat(path, &stbuf);
	IS_ERR(res->err);
	
	if(S_ISREG(stbuf.st_mode)) {
    	FILE *fp;
    	dnfs_stat st;
    	
    	fp = fopen(path, "r+");
    	if(!fp) {
			res->err = -errno;
			return TRUE;
		}
  
    	meta_read_dnfs_stat(&st, fp);
    	
    	/* update mode member */
    	st.s_mode = arg->mode;
    	st.s_ctime = time(NULL);
    	
    	meta_write_dnfs_stat(&st, fp);
    	fclose(fp);
	}
	else if(S_ISDIR(stbuf.st_mode)) {
		res->err = wrapper_chmod(path, arg->mode);
		IS_ERR(res->err);
	}
	
	return TRUE;
}

/*
 * return space information to client
 */
bool_t cs_statfs_1_svc(cs_statfs_in *arg, cs_statfs_out *res, struct svc_req *rqst)
{
	PDEBUG("[cs_statfs_1_svc]: %s\n", arg->path);
	
	res->err = 0;
	res->f_bsize = DNFS_BLK;
	res->f_blocks = nodehead.total * (1024 / DNFS_BLK);
	res->f_bavail = nodehead.avail * (1024 / DNFS_BLK);
	
	return TRUE;
}


/*
 * find a node in the node list
 */
node_info* node_find(uint32_t ip)
{
	int i;
	for(i = 0;  i < nodehead.items; i++) {
		if(nodehead.list[i].ip == ip) {
			return &nodehead.list[i];
		}
	}
	
	return NULL;
}

/*
 * receive the state of each chunk server
 */
bool_t os_heart_1_svc(os_heart_in *arg, os_heart_out *res, struct svc_req *rqst)
{
	res->err = 0;
	
	uint32_t req_ip = get_caller(rqst);
	node_info *item = node_find(req_ip);
	
	/* print debug message */
	char ipp[IP_BUF];
	get_addr(ipp, req_ip);
	PDEBUG("[os_heart_1_svc]: node %s\n", ipp);
	
	if(item) {
		/* 
		 * update space information for dnfs
		 */
		 if(item->hearts < 0) {
		 	/* a broken node goes up */
		 	nodehead.total += arg->total;
		 	nodehead.avail += arg->avail;
		 }
		 else {
		 	nodehead.total += arg->total - item->total;
		 	nodehead.avail += arg->avail - item->avail;
		 }
		 item->total = arg->total;
		 item->avail = arg->avail;
		 
		 item->hearts = 0; 
	}
	else {
		nodehead.items++;
		/* 
		 * double the slots 
		 */
		if(nodehead.items > nodehead.cur_max) {
			nodehead.list = (node_info*)realloc(nodehead.list, 
					     2 * nodehead.cur_max * sizeof(node_info));
			if(!nodehead.list) {
				res->err = -ENOMEM;
				return TRUE;
			}	
			nodehead.cur_max *= 2;
		}
		nodehead.list[nodehead.items-1].ip = req_ip;
		nodehead.list[nodehead.items-1].total = arg->total;
		nodehead.list[nodehead.items-1].avail = arg->avail;
		nodehead.list[nodehead.items-1].hearts = 0;
		
		/* 
		 * update space information for dnfs
		 */
		nodehead.total += arg->total;
		nodehead.avail += arg->avail;
	} 
	
	return TRUE;
}

/*
 * authentic the client
 */
bool_t cs_auth_1_svc(cs_auth_in *arg, cs_auth_out *res, struct svc_req *rqst) 
{
	  res->err = 0;
	  struct spwd *sp;
	  
	  if(strcmp(arg->user, "public")) {  
	  	sp = getspnam(arg->user);
	  	if(strcmp(sp->sp_pwdp, (char*)crypt(arg->passwd, sp->sp_pwdp))) {
	  		res->err = -1;
	  	}
	  }
		
		if(!res->err) {
				char path[PATH_BUF];
				strcpy(path, meta_root);
				strcat(path, "/");
				strcat(path, arg->user);
				check_directory(path);
		}
	  
	  return TRUE;
  
}


int cs_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free(xdr_result, result);
	return 1;
}

int os_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free(xdr_result, result);
	return 1;
}
