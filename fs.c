/*
 * denote the version of fuse to be used;
 * should put before #include <fuse.h>
 */
#define FUSE_USE_VERSION 26  

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#include "cs.h"
#include "client.h"
#include "common.h"
#include "debug.h"
#include "cache.h"

/*
 * the global variables for dnfs
 */

char meta_server[IP_BUF];
unsigned int chunk_size = CHUNK_SIZE;
char thiz_user[AUTH_BUF];

char* get_user_path(const char* path)
{
		char *upath = (char*)malloc(PATH_BUF);
		if(upath == NULL)
				return NULL;
		
		sprintf(upath, "/%s%s", thiz_user, path);
		
		return upath;
}

/* 
 * Get file attributes.
 * Similar to stat().
 * The 'st_dev' and 'st_blksize' fields are ignored. 
 * The 'st_ino' field is ignored except if the 'use_ino' mount option is given. 
 */
static int dnfs_getattr(const char *path, struct stat *stbuf)
{
	PDEBUG("[dnfs_getattr]: %s\n", path);
	
	int ret = 0;
	cs_getattr_in arg;
	cs_getattr_out res;
	
	arg.path = get_user_path(path);
	
	ret = rpc_cs_getattr(&arg, &res, meta_server);
	IS_CALL_ERR(ret);
	
	get_dnfs_attr(stbuf, res.st);
	
	free(arg.path);
	
	return ret;
}

/*
 * Check file access permissions
 * This will be called for the access() system call.
 * If the 'default_permissions' mount option is given, this method is not called.
 */
static int dnfs_access(const char *path, int mode)
{
	PDEBUG("[dnfs_access]: %s\n", path);
	
	int ret = 0;
	cs_access_in arg;
	cs_access_out res;
	
	arg.path = get_user_path(path);
	arg.mode = mode;
	
	ret = rpc_cs_access(&arg, &res, meta_server);
	
	free(arg.path);
	
	return ret;
}

/*
 * Create a directory
 */
static int dnfs_mkdir(const char *path, mode_t mode)
{
	PDEBUG("[dnfs_mkdir]: %s\n", path);
	
	int ret;
	cs_mkdir_in arg;
	cs_mkdir_out res;
	
	arg.path = get_user_path(path);
	arg.mode = mode;
	
	ret = rpc_cs_mkdir(&arg, &res, meta_server);
	
	free(arg.path);
	
	return ret;
}

/*
 * Remove a directory
 */
static int dnfs_rmdir(const char *path)
{
	PDEBUG("[dnfs_rmdir]: %s\n", path);
	
	int ret;
	cs_rmdir_in arg;
	cs_rmdir_out res;
	
	arg.path = get_user_path(path);
	
	ret = rpc_cs_rmdir(&arg, &res, meta_server);
	
	free(arg.path);
	
	return ret;
}

/*
 * Read directory
 * This supersedes the old getdir() interface. 
 * New applications should use this.
 */
static int dnfs_readdir(const char *path, void *buf, 
		fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	PDEBUG("[dnfs_readdir]: %s\n", path);
	
	int ret;
	cs_readdir_in arg;
	cs_readdir_out res;
	
	arg.path = get_user_path(path);
	arg.dp = 0;
	
	res.dirs.dirs_val = (dnfs_dir*)malloc(DIR_BUF * sizeof(dnfs_dir));
	if(!res.dirs.dirs_val) {
		PDEBUG("[dnfs_readdir]: %s\n", strerror(ENOMEM));
		return -ENOMEM;
	}	
	
	res.err = 0;
	while(!res.err) {
		ret = rpc_cs_readdir(&arg, &res, meta_server);
		IS_CALL_ERR(ret);
		
		int i;
		for(i = 0; i < res.dirs.dirs_len; i++) {
			struct stat st;
			memset(&st, 0, sizeof(st));
			st.st_ino = res.dirs.dirs_val[i].d_ino;
        	st.st_mode = res.dirs.dirs_val[i].d_type << 12;
        	if(filler(buf, res.dirs.dirs_val[i].d_name, &st, 0)) {
        		PDEBUG("[dnfs_readdir]: %s\n", strerror(ENOBUFS));
        		return -ENOBUFS;
        	}
		}
		arg.dp = res.dp; 
	}
	
	free(res.dirs.dirs_val);
	free(arg.path);
	
	/* 1 is a finish flag */
	if(ret == 1)
		return 0;
		
	return ret;
}

/*
 * Create a file node
 * This is called for creation of all non-directory, non-symlink nodes. 
 * If the filesystem defines a create() method, then for regular files that will be called instead
 */
static int dnfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
	PDEBUG("[dnfs_mknod] : %s\n", path);
	
	int ret;
	cs_mknod_in arg;
	cs_mknod_out res;
	
	arg.path = get_user_path(path);
	arg.mode = mode;
	
	ret = rpc_cs_mknod(&arg, &res, meta_server);
	
	free(arg.path);
	
	return ret;
}

/*
 * update the server counter in file_info
 */
int set_srv_flag(cache_item *item, int promote)
{
	PDEBUG("[set_srv_flag]: %s\n", item->path);
	
	int ret;
	cs_setwflag_in arg;
	cs_setwflag_out res;
	
	arg.path = (char*)(item->path);
	arg.promote =promote;
	
	ret = rpc_cs_setwflag(&arg, &res, meta_server);
	
	return ret;
}

/*
 * update the client counter in cache item
 * promote = 1 ---> there are no writers before, now a writer comes;
 * promote = -1 ---> there are no writers any more in this client;
 */
int update_counter(cache_item *item, int flags)
{
	PDEBUG("[update_counter]: %s\n", item->path);
	int ret = 0;
	if((flags &O_ACCMODE) == O_RDONLY) {
		item->readers++;
	}
	else {
		if(item->writers == 0) {
			ret = set_srv_flag(item, 1);
			IS_CALL_ERR(ret);
		}
		item->writers++;
	}
	
	return ret;
}

int set_write_flag(cache_item *item)
{
	PDEBUG("[set_write_flag]: %s\n", item->path);
	
	if(!item->wbuf.buf) {
		item->wbuf.buf = (char*)malloc(WRITE_BUF);
		if(!item->wbuf.buf) 
		return -ENOMEM;		
	}
	item->isdirty = 1;
	return 0;
}

void clear_write_flag(cache_item *item)
{
	PDEBUG("[clear_write_flag]: %s\n", item->path);
	/*
	if(item->wbuf.buf) {
		free(item->wbuf.buf);
		item->wbuf.buf = NULL;
	}
	*/
	item->isdirty = 0;
}

/*
 * File open operation
 * obtain a file's size, chunks, ino and chunklist
 */
static int dnfs_open(const char *path, struct fuse_file_info *fi)
{
	PDEBUG("[dnfs_open]: %s\n", path);
	
	int ret = 0;
	
	/*  find the target file's information from cache list */
	char* upath = get_user_path(path);
	cache_item *item = cache_find(upath);

	/*  check if the file have been opend  */
	if(item) {
		/* store cache's address, so read/write/release can use it */
		fi->fh = (unsigned long)item; 
		if((fi->flags & O_ACCMODE) != O_RDONLY) {
			ret = set_write_flag(item);
			IS_CALL_ERR(ret);
		}
		return update_counter(item, fi->flags);
	}
	
	/* first access this file, create a cache item */
	cs_open_in arg;
	cs_open_out res;
	
	arg.path = upath;
	arg.mode = fi->flags;
	
	res.chunks.chunks_val = (chunk_info*)malloc(CHUNK_BUF * sizeof(chunk_info));
	if(!res.chunks.chunks_val) 
		return -ENOMEM;
	
	ret = rpc_cs_open(&arg, &res, meta_server);
	
	if(ret < 0) {
		free(res.chunks.chunks_val);
		return ret;
	}
	
	/* 
	 * return successfully, pass the permission check
	 */
	
	/* get and initialize a cache item */
	item = cache_get();
	
	/* set item members */
	item->path = (char*)malloc(strlen(path)+1);
	if(!item->path) 
		return -ENOMEM;
	strcpy(item->path, arg.path);
	item->ino = res.ino;
	item->size = res.size;
	
	/*
	 * use an unused member for keep ino 
	 * it's the first keyword
	 * it will be used for read/write/release
	 */
	fi->fh = (unsigned long)item; 
	if((fi->flags & O_ACCMODE) != O_RDONLY) {
		ret = set_write_flag(item);
		IS_CALL_ERR(ret);
	}
	ret = update_counter(item, fi->flags);
		IS_CALL_ERR(ret);
	
	
	/* set file chunks information */
	item->chunks = res.chunks.chunks_len;
	item->chunklist = (chunk_info*)malloc(
				item->chunks * sizeof(chunk_info));
	if(!item->chunklist) 
		return -ENOMEM;
	
	int i;
	for(i = 0; i < res.chunks.chunks_len; i++) 
		item->chunklist[i] = res.chunks.chunks_val[i];
	free(res.chunks.chunks_val);
	
	/* add a cache item into the cache list */
	ret = cache_add_head(item);
	
	free(arg.path);
	
	return ret;
}

/*
 * update the data in write buffer to storage node
 * return 0 if the process is successfully finished
 * or errno is returned
 */
static int update_data(cache_item *item)
{
	PDEBUG("[update_data]: %s\n", item->path);
	
    /* no data in the write buffer */
	if(!item->wbuf.size) 
		return 0;
		
	int ret;
	co_write_in arg;
	co_write_out res;
	char ipp[IP_BUF];
	
	int nbr = item->wbuf.offset / chunk_size;
	get_addr(ipp, item->chunklist[nbr].ip);
		
	/* chunkid is ino+chunkno*/
	sprintf(arg.chunkid, "%llu-%u", item->ino, nbr);
		
	arg.offset = item->wbuf.offset % chunk_size;
	arg.buf.buf_val = item->wbuf.buf;
	arg.buf.buf_len = item->wbuf.size;
	arg.ip = item->chunklist[nbr].bip;
		
	ret = rpc_co_write(&arg, &res, ipp);
	IS_CALL_ERR(ret);
	
	/* clear the write buffer */
	item->wbuf.size = 0;
	
	return ret;
}

/* 
 * write the data to write buffer or storage node
 * the data written belongs to one chunk
 * return the data size written
 */
static int write_data(const char *buf, size_t size, off_t offset, 
				cache_item *item)
{
	PDEBUG("[write_data]: %s\n", item->path);
	
	if(!item->wbuf.size) {
		/* 
		 * new data comes and the buffer is empty
		 * just write it to buffer
		 */
		memcpy(item->wbuf.buf, buf, size);
		item->wbuf.offset = offset;
		item->wbuf.size = size;
		
	}
	else if(item->wbuf.size 
		&& offset / chunk_size == item->wbuf.offset / chunk_size
		&& item->wbuf.offset + item->wbuf.size == offset
		&& item->wbuf.size + size <= WRITE_BUF) {
		/*
		 * sequential data with the data in buffer
		 * just add it to buffer
		 */
		memcpy(item->wbuf.buf + item->wbuf.size, buf, size);	
		item->wbuf.size += size;
	}
	else {
		/* 
		 * update the buffer data
		 * write new data to buffer
		 * return 0 on error
		 */
		int ret = update_data(item);
		if(ret < 0)
			return 0;
			
		memcpy(item->wbuf.buf, buf, size);
		item->wbuf.offset = offset;
		item->wbuf.size = size;
	}
	
	return size;
}
/*
 * apply a new chunk 
 * create client handle 
 * and update the cache item
 */
static int apply_chunk(const char *path, cache_item *item)
{
	PDEBUG("[apply_chunk] %s\n", path);
	
	int ret;
	cs_newchunk_in  arg;
	cs_newchunk_out res;
	
	arg.path = get_user_path(path);
	
	ret = rpc_cs_newchunk(&arg, &res, meta_server);
	IS_CALL_ERR(ret);
	
	/* 
	 * no storage node available
	 */
	if(!res.chunk.ip)
		return -ENXIO;
	
	item->chunks++;

	/* realloc chunklist */
	item->chunklist = (chunk_info*)realloc
		(item->chunklist, item->chunks * sizeof(chunk_info));
	if(!item->chunklist) 
		return -ENOMEM;
		
	/* 
	 * fill a new handle, just fill ip member 
	 * the handle is created in update_data
	 */
	item->chunklist[item->chunks-1] = res.chunk;
	
	free(arg.path);
	
	return 0;
}

/*
 * Write data to an open file
 */
static int dnfs_write(const char *path, const char *buf, 
		size_t size, off_t offset, struct fuse_file_info *fi)
{
	PDEBUG("[dnfs_write]: %s, %u, %u\n",
			 path, (unsigned int)offset, (unsigned int)size);
	
	cache_item *item = (cache_item*)((unsigned long)fi->fh);

	/*
	 * o1 represents the start chunk number
	 * o2 represents the end chunk number
	 * for the reason of limited request size and big chunk size 
	 * a request cross two chunks at most
	 */

	int ret;
	int o1, o2; 
	o1 = offset / chunk_size;
	o2 = (offset + size) / chunk_size;
	
	/* just full block */
	if(!((offset + size) % chunk_size)) 
		o2--; 
	
	int written = 0;
	if(o1 == o2 && o2 == item->chunks) {
		/*
		 * the data to write distributed in one chunk
		 * and this chunk need to be created
		 */
		ret = apply_chunk(path, item);
		IS_CALL_ERR(ret);
		
		written += write_data(buf, size, offset, item);
	}
	else if(o1 < o2 && o2 == item->chunks) 
	{
		/*
		 * the data to write distributed in two chunks
		 * and the second chunk need to be created
		 */
		
		/* write data to the first chunk */
		written += write_data(buf, chunk_size - offset % chunk_size, offset, item);
		
		offset += written;
		size -= written;
	
	    /* 
	     *  apply a new chunk
	     * write data to the second chunk 
	     */
	     ret = apply_chunk(path, item);
		 IS_CALL_ERR(ret);
		 
		 written += write_data(buf + written, size, offset, item);
		
	}
	else if(o1 == o2 && o2 < item->chunks) 
	{
		/*
		 * the data to write distributed in one chunk
		 * and this chunk exsits
		 */
		 written += write_data(buf, size, offset, item);
	}
	else if(o1 < o2 && o2 < item->chunks) 
	{
		/*
		 * the data to write distributed in two chunks
		 * and both chunks exsit
		 */
		 	 
		/* write data to the first chunk */
		written += write_data(buf, chunk_size - offset % chunk_size, offset, item);
		
		offset += written;
		size -= written;
		
		/* write data to the second chunk */
		written += write_data(buf + written, size, offset, item);
	}
	
	/* update the filesize */
	if(offset + size > item->size)
			item->size = offset + size;
		
	return written;
			 
}

/*
 * shift the nbr-th item's ip and bip
 */
void promote_backup(cache_item *item, int nbr)
{
	int tmp = item->chunklist[nbr].ip;
	item->chunklist[nbr].ip = item->chunklist[nbr].bip;
	item->chunklist[nbr].bip = tmp; 
}

/* 
 * read data from specified chunk
 */
static int read_data(char *buf, size_t size, off_t offset, 
				cache_item *item)
{
	PDEBUG("[read_data]\n");
	
	int ret;
	co_read_in arg;
	co_read_out res;
	char ipp[IP_BUF];
	
	int nbr = offset / chunk_size;
	get_addr(ipp, item->chunklist[nbr].ip);
		
	/* chunkid is ino+chunk-number */
	sprintf(arg.chunkid, "%llu-%u", item->ino, nbr);
	
	arg.offset = offset % chunk_size;
	arg.size = size;
	
	res.buf.buf_val = buf;
	
	if((ret = rpc_co_read(&arg, &res, ipp)) < 0) {
		if(item->chunklist[nbr].bip) {
			promote_backup(item, nbr);
			get_addr(ipp, item->chunklist[nbr].ip);
			ret = rpc_co_read(&arg, &res, ipp);
			IS_CALL_ERR(ret);
		}
	}
	
	return size;
}

/*
 * Read data from an open file
 */
static int dnfs_read(const char *path, char *buf, 
		size_t size, off_t offset, struct fuse_file_info *fi)
{
	PDEBUG("[dnfs_read]: %s, %u, %u\n", 
			path, (unsigned int)offset, (unsigned int)size);
	
	cache_item *item = (cache_item*)((unsigned long)fi->fh);
	
	int o1, o2; 
	o1 = offset / chunk_size;
	o2 = (offset + size) / chunk_size;
	
	/* request range ends at the end of chunk */
	if(!((offset + size) % chunk_size )) 
		o2--; 

	int read = 0;
	if(o1 == o2) {
		read += read_data(buf, size, offset, item);
		IS_CALL_ERR(read);
	}
	else if(o1 < o2) {
		read += read_data(buf, chunk_size - offset % chunk_size, offset, item);
		IS_CALL_ERR(read);
		
		buf += read;
		offset += read;
		size -= read;
		
		read += read_data(buf, size, offset, item);
		IS_CALL_ERR(read);
	}
	
	return read;
}

/*
 * release is called on each close() of a file descriptor. 
 * So if a filesystem wants to return write errors in close()
 * and the file has cached dirty data, this is a good place to 
 * write back data and return any errors. 
 * Since many applications ignore close() errors this is not always useful.
 */
static int dnfs_release(const char *path, struct fuse_file_info *fi)
{
	PDEBUG("[dnfs_release]: %s\n", path);
	
	cache_item *item = (cache_item*)((unsigned long)fi->fh);

	int ret = 0;
	if((fi->flags & O_ACCMODE) == O_RDONLY) {
		item->readers--;
	}
	else { 
		if(item->writers == 1) {
			ret = set_srv_flag(item, -1);
			IS_CALL_ERR(ret);
		}
		item->writers--;
	}
	
	/* any writer ?? */
	if(item->writers)
		return 0;
		
	/* the data hasn't been changed, simply return */
	if(!item->isdirty) 
		return 0;
	
	/* if write buffer still has data, update it */
	ret = update_data(item);
	IS_CALL_ERR(ret);
	
	/* update file information(size) to server */
	cs_release_in arg;
	cs_release_out res;

	arg.size = item->size;
	arg.path = get_user_path(path);
	
	ret = rpc_cs_release(&arg, &res, meta_server);
	IS_CALL_ERR(ret);
	
	clear_write_flag(item);
	
	free(arg.path);
	
	return ret;
}

/*
 * Remove a file (meta file and data file)
 */
static int dnfs_unlink(const char *path)
{
	PDEBUG("[dnfs_unlink]: %s\n", path);
	
	int i;
	int ret;
	co_unlink_in arg;
	co_unlink_out res;
	
	/* unlink the data file */
	char* upath = get_user_path(path);
	cache_item *item = cache_find(upath);
	if(item) {
		for(i = 0; i < item->chunks; i++) {
			char ipp[IP_BUF];
			if(item->chunklist[i].ip) {
				get_addr(ipp, item->chunklist[i].ip);
				sprintf(arg.chunkid, "%llu-%u", item->ino, i);
				rpc_co_unlink(&arg, &res, ipp);
			}
			
			if(item->chunklist[i].bip) {
				get_addr(ipp, item->chunklist[i].bip);
				sprintf(arg.chunkid, "%llu-%u", item->ino, i);
				rpc_co_unlink(&arg, &res, ipp);
			}
		}
		/* delete the cache */
		cache_del_item(item);
	}
	else {
		cs_getlist_in arg2;
		cs_getlist_out res2;
	
		arg2.path = get_user_path(path);
		res2.chunks.chunks_val = (chunk_info*)malloc(CHUNK_BUF * sizeof(chunk_info));
		if(!res2.chunks.chunks_val) 
			return -ENOMEM;
	
		ret = rpc_cs_getlist(&arg2, &res2, meta_server);
		IS_CALL_ERR(ret);
		
		for(i = 0; i < res2.chunks.chunks_len; i++) {
			char ipp[IP_BUF];
			if(res2.chunks.chunks_val[i].ip) {
				get_addr(ipp, res2.chunks.chunks_val[i].ip);
				sprintf(arg.chunkid, "%llu-%u", res2.ino, i);
				rpc_co_unlink(&arg, &res, ipp);
			}
			
			if(res2.chunks.chunks_val[i].bip) {
				get_addr(ipp, res2.chunks.chunks_val[i].bip);
				sprintf(arg.chunkid, "%llu-%u", res2.ino, i);
				rpc_co_unlink(&arg, &res, ipp);
			}
		}
		free(arg2.path);
	}
	
	/* unlink the metafile */
	cs_unlink_in arg3;
	cs_unlink_out res3;
	
	arg3.path = get_user_path(path);
	ret = rpc_cs_unlink(&arg3, &res3, meta_server);
	
	free(arg3.path);
	free(upath);
	
	return ret;
}

/*
 * Change the size of a file 
 * just support change it's size to 0
 */
static int dnfs_truncate(const char *path, off_t size)
{
	PDEBUG("[dnfs_truncate]: %s\n", path);
	
	if(size > 0)
		return 0;
	
	int ret;
	cs_truncate_in arg;
	cs_truncate_out res;
	
	arg.path = get_user_path(path);
	arg.size = size;
	
	ret = rpc_cs_truncate(&arg, &res, meta_server);
	
	free(arg.path);
	
	return ret;
}

/*
 * Change the owner and group of a file 
 */
static int dnfs_chown(const char *path, uid_t owner, gid_t group)
{
	PDEBUG("[dnfs_chown]: %s\n", path);
	
	int ret;
	cs_chown_in arg;
	cs_chown_out res;
	
	arg.path = get_user_path(path);
	arg.owner = owner;
	arg.group = group;
	
	ret = rpc_cs_chown(&arg, &res, meta_server);
	
	free(arg.path);	
		
	return ret;
}

/*
 * Change the permission bits of a file
 */
static int dnfs_chmod(const char *path, mode_t mode)
{
	PDEBUG("[dnfs_chmod]: %s\n", path);
	
	int ret;
	cs_chmod_in arg;
	cs_chmod_out res;
	
	arg.path = get_user_path(path);
	arg.mode = mode;
	
	ret = rpc_cs_chmod(&arg, &res, meta_server);
	
	free(arg.path);
	return ret;
}  
/*
 * Get file system statistics
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' are ignored
 */
static int dnfs_statfs(const char *path, struct statvfs *stbuf)
{
	PDEBUG("[dnfs_statfs]: %s\n", path);
	
	int ret;
	cs_statfs_in arg;
	cs_statfs_out res;
	
	arg.path = get_user_path(path);
	
	ret = rpc_cs_statfs(&arg, &res, meta_server);
	IS_CALL_ERR(ret);
	
	/* 
	 * set file system information
	 */
	stbuf->f_bsize = res.f_bsize;
	stbuf->f_blocks = res.f_blocks;
	stbuf->f_bavail = res.f_bavail;
	stbuf->f_bfree = res.f_bavail;
	
	free(arg.path);
	
	return ret;	
}

static struct fuse_operations dnfs_ops = {
	.getattr  = dnfs_getattr,
	.access   = dnfs_access,
	.mkdir	  = dnfs_mkdir,
	.rmdir	  = dnfs_rmdir,
	.readdir  = dnfs_readdir,
	.mknod    = dnfs_mknod,
	.open	  = dnfs_open,
	.read	  = dnfs_read,
	.write	  = dnfs_write,
	.release  = dnfs_release,
	.unlink	  = dnfs_unlink,
	.truncate = dnfs_truncate,
	.chown    = dnfs_chown,
	.chmod    = dnfs_chmod,
	.statfs   = dnfs_statfs
};

void usage()
{
	printf("dnfs-mount -s server-ip [-o option] mount-point\n");
}

/* 
 * -s serverip option specifies the meta-data server 
 */
int parse_clnt_cmd(int *argcp, char *argv[])
{
	int argc = *argcp;
	
	if(strcmp(argv[1], "-s") || !is_ipv4(argv[2])) {
		usage();
		return -1;
	}
	strcpy(meta_server, argv[2]);
	printf("metadata server: %s\n", meta_server);
	
	int i;
	for(i = 3; i < argc; i++) {
		argv[i-2] = argv[i];
	}
	
	*argcp = argc - 2;
	
	return 0;
}

int do_auth()
{
	struct termios ts, ots;
	char user[AUTH_BUF];
	char passwd[AUTH_BUF];
	int auth_times = 3;
	
	
	int suc = -1;  
  while(auth_times--) {
  	tcgetattr(STDIN_FILENO, &ts);
		ots = ts;

		printf("Login:");
	  fflush(stdout);
	  gets(user);
	  
	  if(user[0]== '\0') {
	  	strcpy(user, "public");
	  	passwd[0] = '\0';
	  }
	  else {
	  	/* close echo */
			ts.c_lflag &= ~ECHO;
			ts.c_lflag |= ECHONL;
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts);

			printf("Password:");
			fflush(stdout);
			gets(passwd);
	  }
	 
		cs_auth_in arg;
		cs_auth_out res;
		arg.user = user;
		arg.passwd = passwd;
		int ret = rpc_cs_auth(&arg, &res, meta_server);
		
		tcsetattr(STDIN_FILENO, TCSANOW, &ots);
		
		if(!ret) {
			suc = 0;
			strcpy(thiz_user, user);
			break;
		}
  }
  
  return suc;
}

int main(int argc, char *argv[])
{
	
	/* check parameter */
	if(argc < 4) {
		usage();
		return -1;
	}
	
	/* parse command line arguement */
	if(parse_clnt_cmd(&argc, argv))
		return -1;
	
	/* authentication */
	if(do_auth())
		return -1;
	
	/* initialize cache list */
	cache_init();
	
    return fuse_main(argc, argv, &dnfs_ops, NULL);
}

