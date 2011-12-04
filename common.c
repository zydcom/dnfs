#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "common.h"

/********************************************************
 ******* attribute related funciton implementation ******
 ********************************************************/  

/*
 * convert a stat struct to dnfs_stat
 */
void set_dnfs_attr(struct stat stbuf, dnfs_stat *st)
{
   st->s_ino     = stbuf.st_ino;
   st->s_mode    = stbuf.st_mode;
   st->s_nlink   = stbuf.st_nlink;
   st->s_uid     = stbuf.st_uid;
   st->s_gid     = stbuf.st_gid;
   st->s_rdev    = stbuf.st_rdev;
   st->s_size    = stbuf.st_size;
   st->s_blocks  = stbuf.st_blocks;
   st->s_atime   = stbuf.st_atime;
   st->s_mtime   = stbuf.st_mtime;
   st->s_ctime   = stbuf.st_ctime;
}

/*
 * convert a dnfs_stat to struct stat
 */
void get_dnfs_attr(struct stat *stbuf, dnfs_stat st)
{
   stbuf->st_ino     = st.s_ino;
   stbuf->st_mode    = st.s_mode;
   stbuf->st_nlink   = st.s_nlink;
   stbuf->st_uid     = st.s_uid;
   stbuf->st_gid     = st.s_gid;
   stbuf->st_rdev    = st.s_rdev;
   stbuf->st_size    = st.s_size;
   stbuf->st_blocks  = st.s_blocks;
   stbuf->st_atime   = st.s_atime;
   stbuf->st_mtime   = st.s_mtime;
   stbuf->st_ctime   = st.s_ctime;
   /* 
    * below items are ignored by fuse
    */
   stbuf->st_dev     = 0;
   stbuf->st_blksize = 0;
}


/********************************************************
 ******* ip address related funciton implementation *****
 ********************************************************/  

#define PART_ONE  0x1000000 
#define PART_TWO  0x10000 
#define PART_THREE 0x100

/*
 * weakly check the ip arg is an ipv4 address 
 */
 
int is_ipv4(char *ip)
{
	#define is_byte(b) ((b) >= 0) && ((b) <= 255)
	
	unsigned int p1, p2, p3, p4;
	if( sscanf(ip, "%u.%u.%u.%u", &p1, &p2, &p3, &p4) != 4 ) {
		return 0;
	}
	
	return ( is_byte(p1) && is_byte(p2) && 
			is_byte(p3) && is_byte(p4) && p1 );
}

/*
 * put an ip string to an integer
 */
void put_addr(char *ipp, uint32_t *ip)
{
    unsigned int p1, p2, p3, p4;
    sscanf(ipp, "%u.%u.%u.%u", &p1, &p2, &p3, &p4);
    *ip = p1 * PART_ONE + p2 * PART_TWO + p3 * PART_THREE + p4;
}

/*
 * get an ip string from an integer
 */
void get_addr(char *ipp, uint32_t ip)
{
    unsigned int p1, p2, p3, p4;
    p1 = ip / PART_ONE;
    p2 = ip % PART_ONE / PART_TWO;
    p3 = ip % PART_TWO / PART_THREE;
    p4 = ip % PART_THREE;
    sprintf(ipp, "%u.%u.%u.%u", p1, p2, p3, p4);
}


/* 
 * check as a root.
 * meta_root exsits??
 * if not, create it recursively.
 */ 
int check_directory(char *path)
{
	int res = 0;
	res = access(path, F_OK);
	if(res < 0) {
		char root_dir[PATH_BUF];
		sprintf(root_dir, "mkdir -p %s", path);
		if(system(root_dir) < 0)
			return -1;
	}
	return 0;
}

