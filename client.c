#include "client.h"
#include "debug.h"

/*
 * client wrapper function for rpc call
 * return EREMOTE(a public errno) to denote call error 
 * two errors: crete client handle or call client function
 */

/* 
 * call cs_getattr with udp protocol
 */
int rpccall_cs_getattr(cs_getattr_in *arg, cs_getattr_out *res, char *ipp)
{
	CLIENT *clnt;

	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_getattr_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_getattr_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_getattr(cs_getattr_in *arg, cs_getattr_out *res, char *ipp)
{
	int ret = rpccall_cs_getattr(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_access with udp protocol
 */
int rpccall_cs_access(cs_access_in *arg, cs_access_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_access_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_access_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}


int rpc_cs_access(cs_access_in *arg, cs_access_out *res, char *ipp)
{
	int ret = rpccall_cs_access(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}


/* 
 * call cs_mkdir with udp protocol
 */
int rpccall_cs_mkdir(cs_mkdir_in *arg, cs_mkdir_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_mkdir_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_mkdir_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_mkdir(cs_mkdir_in *arg, cs_mkdir_out *res, char *ipp)
{
	int ret = rpccall_cs_mkdir(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}


/* 
 * call cs_rmdir with udp protocol
 */
int rpccall_cs_rmdir(cs_rmdir_in *arg, cs_rmdir_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_rmdir_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_rmdir_1 failed"), ipp);
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_rmdir(cs_rmdir_in *arg, cs_rmdir_out *res, char *ipp)
{
	int ret = rpccall_cs_rmdir(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_readdir with udp protocol
 */
int rpccall_cs_readdir(cs_readdir_in *arg, cs_readdir_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_readdir_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_readdir_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_readdir(cs_readdir_in *arg, cs_readdir_out *res, char *ipp)
{
	int ret = rpccall_cs_readdir(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_mknod with udp protocol
 */
int rpccall_cs_mknod(cs_mknod_in *arg, cs_mknod_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_mknod_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_mknod_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_mknod(cs_mknod_in *arg, cs_mknod_out *res, char *ipp)
{
	int ret = rpccall_cs_mknod(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_newchunk with udp protocol
 */
int rpccall_cs_newchunk(cs_newchunk_in *arg, cs_newchunk_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_newchunk_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_newchunk_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_newchunk(cs_newchunk_in *arg, cs_newchunk_out *res, char *ipp)
{
	int ret = rpccall_cs_newchunk(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}


/* 
 * call cs_release with udp protocol
 */
int rpccall_cs_release(cs_release_in *arg, cs_release_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_release_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_release_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_release(cs_release_in *arg, cs_release_out *res, char *ipp)
{
	int ret = rpccall_cs_release(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_getlist with tcp protocol
 */
int rpccall_cs_getlist(cs_getlist_in *arg, cs_getlist_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "tcp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_getlist_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_getlist_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_getlist(cs_getlist_in *arg, cs_getlist_out *res, char *ipp)
{
	int ret = rpccall_cs_getlist(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_unlink with udp protocol
 */
int rpccall_cs_unlink(cs_unlink_in *arg, cs_unlink_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_unlink_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_unlink_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_unlink(cs_unlink_in *arg, cs_unlink_out *res, char *ipp)
{
	int ret = rpccall_cs_unlink(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_truncate with udp protocol
 */
int rpccall_cs_truncate(cs_truncate_in *arg, cs_truncate_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_truncate_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_truncate_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_truncate(cs_truncate_in *arg, cs_truncate_out *res, char *ipp)
{
	int ret = rpccall_cs_truncate(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_chown with udp protocol
 */
int rpccall_cs_chown(cs_chown_in *arg, cs_chown_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_chown_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_chown_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_chown(cs_chown_in *arg, cs_chown_out *res, char *ipp)
{
	int ret = rpccall_cs_chown(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_chmod with udp protocol
 */
int rpccall_cs_chmod(cs_chmod_in *arg, cs_chmod_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_chmod_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_chmod_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_chmod(cs_chmod_in *arg, cs_chmod_out *res, char *ipp)
{
	int ret = rpccall_cs_chmod(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_statfs with udp protocol
 */
int rpccall_cs_statfs(cs_statfs_in *arg, cs_statfs_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_statfs_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_statfs_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_statfs(cs_statfs_in *arg, cs_statfs_out *res, char *ipp)
{
	int ret = rpccall_cs_statfs(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_setwflag with udp protocol
 */
int rpccall_cs_setwflag(cs_setwflag_in *arg, cs_setwflag_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_setwflag_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_setwflag_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_setwflag(cs_setwflag_in *arg, cs_setwflag_out *res, char *ipp)
{
	int ret = rpccall_cs_setwflag(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_auth with udp protocol
 */
int rpccall_cs_auth(cs_auth_in *arg, cs_auth_out *res, char *ipp)
{
	CLIENT *clnt;

	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "udp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_auth_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_auth_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_auth(cs_auth_in *arg, cs_auth_out *res, char *ipp)
{
	int ret = rpccall_cs_auth(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call cs_open with tcp protocol
 */
int rpccall_cs_open(cs_open_in *arg, cs_open_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CS_PROG, CS_VERS, "tcp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(cs_open_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call cs_open_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_cs_open(cs_open_in *arg, cs_open_out *res, char *ipp)
{
	int ret = rpccall_cs_open(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call co_read with tcp protocol
 */
int rpccall_co_read(co_read_in *arg, co_read_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CO_PROG, CO_VERS, "tcp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(co_read_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call co_read_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_co_read(co_read_in *arg, co_read_out *res, char *ipp)
{
	int ret = rpccall_co_read(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call co_write with tcp protocol
 */
int rpccall_co_write(co_write_in *arg, co_write_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CO_PROG, CO_VERS, "tcp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(co_write_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call co_write_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_co_write(co_write_in *arg, co_write_out *res, char *ipp)
{
	int ret = rpccall_co_write(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * call co_write with tcp protocol
 */
int rpccall_co_unlink(co_unlink_in *arg, co_unlink_out *res, char *ipp)
{
	CLIENT *clnt;
	
	clnt = clnt_create(ipp, CO_PROG, CO_VERS, "tcp");
	if(!clnt) {
		PDEBUG("%s: create client handle failed\n", ipp);
		return -EREMOTE;
	}
	
	if(co_unlink_1(arg, res, clnt) != RPC_SUCCESS) {
		PDEBUG(clnt_sperror(clnt, "call co_unlink_1 failed"));
		return -EREMOTE;
	}
	
	clnt_destroy(clnt);
	
	return 0;
}

int rpc_co_unlink(co_unlink_in *arg, co_unlink_out *res, char *ipp)
{
	int ret = rpccall_co_unlink(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}