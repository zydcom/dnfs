const PATH_BUF  = 1024;        /* path name length */         
const DIR_BUF   = 30;         /* 8192/sizeof(dnfs_dir) items*/
const CHUNK_BUF = 1024;       /* max chunk size, support 64G file size */
const AUTH_BUF = 256;        /* for authentication */

/* 
 * dnfs file stat struct 
 * this struct will be stored in metadata server
 * it must be platform independent
 */
struct dnfs_stat {
   uint64_t s_ino;
   uint32_t s_mode;
   uint16_t s_chunks;
   uint16_t s_copies;
   uint64_t s_nlink;
   uint32_t s_uid;
   uint32_t s_gid;
   uint64_t s_rdev;
   uint64_t s_size;
   uint64_t s_blocks;
   uint64_t s_atime;
   uint64_t s_mtime;
   uint64_t s_ctime;
};

/*
 * identify a strage chunk and it's copy
 */
struct chunk_info {
	uint32_t ip;
	uint32_t bip;
};

/*
 * dnfs directory struct
 */
struct dnfs_dir {
	uint64_t d_ino;
	uint8_t d_type;
	opaque d_name[PATH_BUF];
};

/*
 * structs for client-server rpc program
 */
 
struct cs_getattr_in {
	string path<PATH_BUF>;
};

struct cs_getattr_out {
   dnfs_stat st;
   int  err;
};

struct cs_access_in {
	string path<PATH_BUF>;
	int mode;
};

struct cs_access_out {
	int err;
};

struct cs_mkdir_in {
	string path<PATH_BUF>;
	unsigned int mode;
};

struct cs_mkdir_out {
	int err;
};

struct cs_rmdir_in {
	string path<PATH_BUF>;
};

struct cs_rmdir_out {
	int err;
};

struct cs_readdir_in
{
	string path<PATH_BUF>;
	unsigned long dp;
};


struct cs_readdir_out {
	dnfs_dir dirs<DIR_BUF>;
	unsigned long dp;
	int err;
};

struct cs_mknod_in {
	string path<PATH_BUF>;
	unsigned int mode;
};

struct cs_mknod_out {
	int err;
};

struct cs_setwflag_in {
	string path<PATH_BUF>;
	int promote;
};

struct cs_setwflag_out {
	int err;
};

struct cs_open_in {
	string path<PATH_BUF>;
	unsigned int mode;
};

struct cs_open_out {
	uint64_t ino;
	uint64_t size;
	chunk_info chunks<CHUNK_BUF>;
	int err;
};

struct cs_newchunk_in {
	string path<PATH_BUF>;
};

struct cs_newchunk_out {	
	chunk_info chunk;
	int err;
};

struct cs_release_in {
	string path<PATH_BUF>;
	uint64_t size;
};

struct cs_release_out {
	int err;
};


struct cs_truncate_in {
	string path<PATH_BUF>;
    uint64_t size;
};

struct cs_truncate_out {
	int err;
};

struct cs_getlist_in {
	string path<PATH_BUF>;
};

struct cs_getlist_out {
	uint64_t ino;
	chunk_info chunks<CHUNK_BUF>;
	int err;
};

struct cs_unlink_in {
	string path<PATH_BUF>;
};

struct cs_unlink_out {
	int err;
};

struct cs_chown_in {
	string path<PATH_BUF>;
	unsigned int owner;
	unsigned int group;
};

struct cs_chown_out {
	int err;
};

struct cs_chmod_in {
	string path<PATH_BUF>;
	unsigned int mode;
};

struct cs_chmod_out {
	int err;
};

struct cs_statfs_in {
	string path<PATH_BUF>;
};

struct cs_statfs_out {
	uint32_t f_blocks;
	uint32_t f_bavail;
	uint32_t f_bsize;
	int err;
};

struct cs_auth_in {
	string user<AUTH_BUF>;
	string passwd<AUTH_BUF>;
};

struct cs_auth_out {
	int err;
};

/*
 * client server program definition 
 */
program CS_PROG {
	version CS_VERS {
		cs_getattr_out 	 cs_getattr(cs_getattr_in)   = 1;
		cs_access_out 	 cs_access(cs_access_in)     = 2;
		cs_mkdir_out  	 cs_mkdir(cs_mkdir_in)       = 3;
		cs_rmdir_out     cs_rmdir(cs_rmdir_in)  	 = 4;
		cs_readdir_out   cs_readdir(cs_readdir_in)   = 5;
		cs_mknod_out     cs_mknod(cs_mknod_in)       = 6;
		cs_open_out      cs_open(cs_open_in)         = 7;
		cs_newchunk_out  cs_newchunk(cs_newchunk_in) = 8;
		cs_release_out   cs_release(cs_release_in)   = 9;
		cs_unlink_out    cs_unlink(cs_unlink_in) 	 = 10;
		cs_truncate_out  cs_truncate(cs_truncate_in) = 11;
		cs_chown_out	 cs_chown(cs_chown_in)		 = 12;
		cs_chmod_out     cs_chmod(cs_chmod_in)       = 13;
		cs_statfs_out	 cs_statfs(cs_statfs_in)     = 14;
		cs_getlist_out   cs_getlist(cs_getlist_in)   = 15;
		cs_setwflag_out cs_setwflag(cs_setwflag_in) = 16;
		cs_auth_out     cs_auth(cs_auth_in)         = 17;
		
	} = 1;
} = 0x31230001;

/*
 * structs for osd-server rpc program
 */
 
struct os_heart_in {
	uint32_t total;
	uint32_t avail;
};

struct os_heart_out {
	int err;
};

program OS_PROG {
	version OS_VERS {
		os_heart_out os_heart(os_heart_in) = 1;
	} = 1;
} = 0x31230003;

