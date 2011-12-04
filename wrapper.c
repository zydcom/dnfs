#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include "wrapper.h"
#include "debug.h"

/*
 * all the functions are wrappers
 * with errno as a return value
 */
int wrapper_lstat(const char *path, struct stat * stbuf)
{
	int res = 0;
	res = lstat(path, stbuf);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_lstat]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_open(const char *path, int flag, int *fd)
{
	int res = 0;
	res = open(path, flag);
	*fd = res;
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_open]: %s\n", strerror(errno));
	} 
	return res;
}

int wrapper_open_mode(const char *path, int flag, int mode, int *fd)
{
	int res = 0;
	res = open(path, flag, mode);
	*fd = res;
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_open]: %s\n", strerror(errno));
	} 
	return res;
}

int wrapper_pread(int fd, void *buf, size_t count, off_t offset)
{
	int res = 0;
	res = pread(fd, buf, count, offset);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_read]: %s\n", strerror(errno));
	}		
	return res;
}

int wrapper_pwrite(int fd, void *buf, size_t count, off_t offset)
{
	int res = 0;
	res = pwrite(fd, buf, count, offset);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_pwrite]: %s\n", strerror(errno));
	}		
	return res;
}

int wrapper_close(int fd) 
{
	int res = 0;
	res = close(fd);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_close]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_access(const char *path, int mode)
{
	int res = 0;
	res = access(path, mode);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_access]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_mkdir(const char *path, mode_t mode)
{
	int res = 0;
	res = mkdir(path, mode);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_mkdir]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_rmdir(const char *path)
{
	int res = 0;
	res = rmdir(path);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_rmdir]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_mknod(const char *path, mode_t mode, dev_t dev)
{
	int res = 0;
	res = mknod(path, mode, dev);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_mknod]: %s\n", strerror(errno));
	}
	return res;
	
}

int wrapper_unlink(const char *path) 
{
	int res = 0;
	res = unlink(path);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_unlink]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_truncate(const char *path, off_t length)
{
	int res = 0;
	res = truncate(path, length);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_truncate]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_chown(const char *path, uid_t owner, gid_t group)
{
	int res = 0;
	res = chown(path, owner, group);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_chown]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_chmod(const char *path, mode_t mode)
{
	int res = 0;
	res = chmod(path, mode);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_chmod]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_opendir(const char *path, DIR **dp)
{
	int res = 0;
	*dp = opendir(path);
	if(!dp) {
		res = -errno;
		PDEBUG("[wrapper_opendir]: %s\n", strerror(errno));
	}
	return res;
}

int wrapper_readdir(DIR *dp, struct dirent **dir)
{
	int res = 0;
	
	/* 
	 * set errno to 0,
	 * so we can tell when readdir() fails 
	 */
	errno = 0;
	*dir = readdir(dp);
	if(errno) {
		res = -errno;
		PDEBUG("[wrapper_readdir]: %s\n", strerror(errno));
		return res;
	}
	errno = 0;
	return res;
}

int wrapper_closedir(DIR *dp)
{
	int res = 0;
	res = closedir(dp);
	if(res < 0) {
		res = -errno;
		PDEBUG("[wrapper_closedir]: %s\n", strerror(errno));
	}
	return res;
}




