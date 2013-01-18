#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "co.h"
#include "wrapper.h"
#include "storage.h"
#include "debug.h"

extern char storage_root[PATH_BUF];

/* 
 * convert a pathname, add prefix meta_root 
 */
void get_chunk_path(char *dest, char *src)
{
	sprintf(dest, "%s/%s", storage_root, src);
}

/*
 * write the data to disk
 * return the error information 
 */
void *do_write(void *write_arg)
{
	PDEBUG("[do_write thread]\n");
	co_write_in *arg = (co_write_in*)write_arg;
	
	char *buf 	= arg->buf.buf_val; 
	size_t size = arg->buf.buf_len;
	off_t offset = arg->offset;
	
	char path[PATH_BUF];
	get_chunk_path(path, arg->chunkid);
	
	int fd;
	static int ret = 0;
	ret = wrapper_open_mode(path, O_CREAT | O_WRONLY, 0644, &fd);
	if(ret < 0)
		return (void*)&ret;
  	ret = wrapper_pwrite(fd, buf, size, offset);
  	if(ret < 0)
  		return (void*)&ret;
  	ret = wrapper_close(fd);
  	
  	return (void*)&ret;
}

/* 
 * copy data using rpc
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

int copy_data(co_write_in *arg, co_write_out *res, char *ipp)
{
	int ret = rpccall_co_write(arg, res, ipp);
	if(ret < 0)
		return ret;
	return res->err;
}

/* 
 * copy the data to the backup osd
 * return the error information 
 */
void *do_copy(void *copy_arg)
{
	PDEBUG("[do_copy thread]\n");
	
	static int ret = 0;
	co_write_in *arg = (co_write_in*)copy_arg;
	co_write_out res;
	res.err = 0;
	
	char ipp[IP_BUF];
	get_addr(ipp, arg->ip);
	arg->ip = 0;
	ret = copy_data(arg, &res, ipp);
	
	return (void*)&ret;
}

/*
 * return the bytes written or errno
 */
bool_t co_write_1_svc(co_write_in *arg, co_write_out *res, struct svc_req *rqst)
{	
	PDEBUG("[co_write_1_svc] chunkid = %s, size = %u, offset = %u\n",
				 arg->chunkid, arg->buf.buf_len, arg->offset);
				 
	res->err = 0;
	
	int *ret;
	pthread_t tid1, tid2;
	
	if(pthread_create(&tid1, NULL, do_write, (void*)arg)) {
		return FALSE;
	}
	
	if(arg->ip){
		if(pthread_create(&tid2, NULL, do_copy, (void*)arg)) {
			return FALSE;
		}
		pthread_join(tid2, (void**)&ret);
		res->err = *ret;
		IS_ERR(res->err);
	}
	
	pthread_join(tid1, (void**)&ret);
	res->err = *ret;
	
	return TRUE;	
}

/*
 * return the bytes read or errno
 */
bool_t co_read_1_svc(struct co_read_in *arg, struct co_read_out *res, struct svc_req *rqst)
{		 
	PDEBUG("[co_read_1_svc] chunkid = %s, size = %u, offset = %u\n",
				 arg->chunkid, arg->size, arg->offset);
				 
	size_t size = arg->size;
	off_t offset = arg->offset;
	
	char path[PATH_BUF];
	get_chunk_path(path, arg->chunkid);
	res->err = 0;

	res->buf.buf_val  = (char*)malloc(size);	
	if(!res->buf.buf_val) {
		res->err = -ENOMEM;
		res->buf.buf_len = 0;
		res->buf.buf_val = NULL;
		return TRUE;
	}
	
	int fd;
	res->err = wrapper_open(path, O_RDONLY, &fd);
	if(res->err < 0) {
		res->err = -errno;
		res->buf.buf_len = 0;
		res->buf.buf_val = NULL;
		return 0;
	}
  	res->err = wrapper_pread(fd, res->buf.buf_val, size, offset);
  	IS_ERR(res->err);
  	res->buf.buf_len = size;
  	res->err = wrapper_close(fd);
	
	return TRUE;
}


/*
 * unlink the data file 
 */
bool_t co_unlink_1_svc(struct co_unlink_in *arg, struct co_unlink_out *res, struct svc_req *rqst)
{		 
	PDEBUG("[co_unlink_1_svc] chunkid = %s\n", arg->chunkid);
	
	char path[PATH_BUF];
	get_chunk_path(path, arg->chunkid);
	res->err = 0;
	res->err = wrapper_unlink(path);
	
	return TRUE;
}

int co_prog_1_freeresult(SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free(xdr_result, result);
	return 1;
}