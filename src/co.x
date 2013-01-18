const CID_BUF     = 20;			 /* 12bit ino + serial number */  
const RW_BUF      = 1048576;     /* 1M read/write limit */


/*
 * struct for client-osd program
 */
struct co_write_in {
	opaque chunkid[CID_BUF];
	opaque buf<RW_BUF>;  /* buf and size */
	unsigned long offset;
	uint32_t ip;
};

struct co_write_out {
	int err;
};

struct co_read_in {
	opaque chunkid[CID_BUF];
	unsigned int size;
	unsigned long offset;
};

struct co_read_out {
	opaque buf<RW_BUF>;
	int err;
};

struct co_unlink_in {
	opaque chunkid[CID_BUF];
};

struct co_unlink_out {
	int err;
};

/* 
 * client-osd program definition
 */
program CO_PROG {
	version CO_VERS {
		co_write_out co_write(co_write_in) = 1;
	    co_read_out  co_read(co_read_in)   = 2;
	    co_unlink_out co_unlink(co_unlink_in) = 3;
	} = 1;
} = 0x31230002;