#ifndef _CLIENT_H
#define _CLIENT_H

#include <sys/stat.h>
#include <errno.h>
#include "cs.h"
#include "co.h"

#define IP_BUF       16         /* ip address length */
#define LINE_BUF	 128        /* line buffer for configuration */
#define CHUNK_SIZE   8388608   /* 8M chunk size for media storage */

extern char meta_server[];  			 /* the target meta-data server  */
extern unsigned int chunk_size;      			 /* chunk size */

/*  
 * client function set
 */
int rpc_cs_getattr(cs_getattr_in *arg, cs_getattr_out *res, char *ipp);
int rpc_cs_access(cs_access_in *arg, cs_access_out *res, char *ipp);
int rpc_cs_mkdir(cs_mkdir_in *arg, cs_mkdir_out *res, char *ipp);
int rpc_cs_rmdir(cs_rmdir_in *arg, cs_rmdir_out *res, char *ipp);
int rpc_cs_readdir(cs_readdir_in *arg, cs_readdir_out *res, char *ipp);
int rpc_cs_mknod(cs_mknod_in *arg, cs_mknod_out *res, char *ipp);
int rpc_cs_newchunk(cs_newchunk_in *arg, cs_newchunk_out *res, char *ipp);
int rpc_cs_release(cs_release_in *arg, cs_release_out *res, char *ipp);
int rpc_cs_getlist(cs_getlist_in *arg, cs_getlist_out *res, char *ipp);
int rpc_cs_unlink(cs_unlink_in *arg, cs_unlink_out *res, char *ipp);
int rpc_cs_truncate(cs_truncate_in *arg, cs_truncate_out *res, char *ipp);
int rpc_cs_chown(cs_chown_in *arg, cs_chown_out *res, char *ipp);
int rpc_cs_chmod(cs_chmod_in *arg, cs_chmod_out *res, char *ipp);
int rpc_cs_statfs(cs_statfs_in *arg, cs_statfs_out *res, char *ipp);
int rpc_cs_auth(cs_auth_in *arg, cs_auth_out *res, char *ipp);
int rpc_cs_setwflag(cs_setwflag_in *arg, cs_setwflag_out *res, char *ipp);
int rpc_cs_open(cs_open_in *arg, cs_open_out *res, char *ipp);
int rpc_co_read(co_read_in *arg, co_read_out *res, char *ipp);
int rpc_co_write(co_write_in *arg, co_write_out *res, char *ipp);
int rpc_co_unlink(co_unlink_in *arg, co_unlink_out *res, char *ipp);


#endif

