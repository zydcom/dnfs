
/*
 * common utilities and macro for dnfs
 * it will be used in client, node and server
 */
 
#ifndef _COMMON_H
#define _COMMON_H

/*
 * attribute related functions
 */
#include <sys/stat.h>
#include <unistd.h>
#include "cs.h"
void set_dnfs_attr(struct stat stbuf, dnfs_stat *st);
void get_dnfs_attr(struct stat *stbuf, dnfs_stat st);

/*
 * address related functions
 */
int  is_ipv4(char *ip);
void put_addr(char *ipp, uint32_t *ip);
void get_addr(char *ipp, uint32_t ip);

/* 
 * file related functions
 */
int check_directory(char *path);

#endif