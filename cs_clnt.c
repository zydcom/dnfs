/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "cs.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

enum clnt_stat 
cs_getattr_1(cs_getattr_in *argp, cs_getattr_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_getattr,
		(xdrproc_t) xdr_cs_getattr_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_getattr_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_access_1(cs_access_in *argp, cs_access_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_access,
		(xdrproc_t) xdr_cs_access_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_access_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_mkdir_1(cs_mkdir_in *argp, cs_mkdir_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_mkdir,
		(xdrproc_t) xdr_cs_mkdir_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_mkdir_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_rmdir_1(cs_rmdir_in *argp, cs_rmdir_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_rmdir,
		(xdrproc_t) xdr_cs_rmdir_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_rmdir_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_readdir_1(cs_readdir_in *argp, cs_readdir_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_readdir,
		(xdrproc_t) xdr_cs_readdir_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_readdir_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_mknod_1(cs_mknod_in *argp, cs_mknod_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_mknod,
		(xdrproc_t) xdr_cs_mknod_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_mknod_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_open_1(cs_open_in *argp, cs_open_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_open,
		(xdrproc_t) xdr_cs_open_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_open_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_newchunk_1(cs_newchunk_in *argp, cs_newchunk_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_newchunk,
		(xdrproc_t) xdr_cs_newchunk_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_newchunk_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_release_1(cs_release_in *argp, cs_release_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_release,
		(xdrproc_t) xdr_cs_release_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_release_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_unlink_1(cs_unlink_in *argp, cs_unlink_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_unlink,
		(xdrproc_t) xdr_cs_unlink_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_unlink_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_truncate_1(cs_truncate_in *argp, cs_truncate_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_truncate,
		(xdrproc_t) xdr_cs_truncate_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_truncate_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_chown_1(cs_chown_in *argp, cs_chown_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_chown,
		(xdrproc_t) xdr_cs_chown_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_chown_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_chmod_1(cs_chmod_in *argp, cs_chmod_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_chmod,
		(xdrproc_t) xdr_cs_chmod_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_chmod_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_statfs_1(cs_statfs_in *argp, cs_statfs_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_statfs,
		(xdrproc_t) xdr_cs_statfs_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_statfs_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_getlist_1(cs_getlist_in *argp, cs_getlist_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_getlist,
		(xdrproc_t) xdr_cs_getlist_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_getlist_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_setwflag_1(cs_setwflag_in *argp, cs_setwflag_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_setwflag,
		(xdrproc_t) xdr_cs_setwflag_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_setwflag_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
cs_auth_1(cs_auth_in *argp, cs_auth_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, cs_auth,
		(xdrproc_t) xdr_cs_auth_in, (caddr_t) argp,
		(xdrproc_t) xdr_cs_auth_out, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
os_heart_1(os_heart_in *argp, os_heart_out *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, os_heart,
		(xdrproc_t) xdr_os_heart_in, (caddr_t) argp,
		(xdrproc_t) xdr_os_heart_out, (caddr_t) clnt_res,
		TIMEOUT));
}
