#ifndef _DEBUG_H
#define _DEBUG_H

/*
 * macros to help debugging
 */
#define DNFS_DEBUG         /* define this macro to turn on debug mode */

#ifdef DNFS_DEBUG
	#undef  PDEBUG                                                 /* undef it, just in case */
	#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)     /* user space debug */
#else
	#define PDEBUG(fmt, args...)         /* non debug mode, do nothing  */
#endif

/* 
 * macro for return a errno 
 */
#define IS_CALL_ERR(errno) do {if((errno) < 0) return errno; } while(0)
#define PTR_ERR(errno) do { if((errno) < 0) return (void*)errno; } while(0)

#endif




