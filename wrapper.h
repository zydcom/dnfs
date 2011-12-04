#ifndef _WRAPPER_H
#define _WRAPPER_H

#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>

/*
 * wrapper function for system calls
 * if error occurs, return -errno
 * otherwise, return 0
 */
#define IS_ERR(errno) do { if((errno) < 0) return TRUE; } while(0)
int wrapper_lstat(const char *path, struct stat * stbuf);		
int wrapper_open(const char *path, int flag, int *fd);
int wrapper_open_mode(const char *path, int flag, int mode, int *fd);
int wrapper_pread(int fd, void *buf, size_t count, off_t offset);
int wrapper_pwrite(int fd, void *buf, size_t count, off_t offset);
int wrapper_close(int fd);
int wrapper_access(const char *path, int mode);
int wrapper_mkdir(const char *path, mode_t mode);
int wrapper_rmdir(const char *path);
int wrapper_mknod(const char *path, mode_t mode, dev_t dev);
int wrapper_unlink(const char *path);
int wrapper_truncate(const char *path, off_t length);
int wrapper_chown(const char *path, uid_t owner, gid_t group);
int wrapper_chmod(const char *path, mode_t mode);
int wrapper_opendir(const char *path, DIR **dp);
int wrapper_readdir(DIR *dp, struct dirent **dir);
int wrapper_closedir(DIR *dp);

#endif