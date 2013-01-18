dnfs
====

Distributed Network File System Based on FUSE 

General Information
===================

DNFS (Distributed Network File System) is a userspace file system based 
on FUSE (Filesystem in Userspace).


Dependencies
============

Linux kernel version 2.6.X where X >= 9.
GNU libc library


Installation
============
    fisrt install fuse
    ./configure
    make 
    make install
    modprobe fuse

You may also need to add '/usr/local/lib' to '/etc/ld.so.conf' and/or
run ldconfig.

**then build dnfs**
    run "make dnfs-mount" to build the dnfs client.
    run "make dnfs-server" to build the dnfs server.
    run "make dnfs-node" to build the dnfs node.
    run "make" to build all the three parts.
    run "make install" installs the dnfs bin to /usr/bin.

How To Use
==========
**meta-data server**
    ./dnfs-server  or dnfs-server

**dnfs client**
    ./dnfs-mount option or dnfs-mount option
    eg: ./dnfs-mount -s 192.168.1.72 /mnt/dnfs 

**dnfs storage node**
    ./dnfs-node -s server-ip [other options]
    eg: ./dnfs-node -s 192.168.1.72 or dnfs-node -s 192.168.1.72


DNFS Mount Options
=============

-s specify the server name in ip address format.

Most of the generic mount options described in 'man mount' are
supported (ro, rw, suid, nosuid, dev, nodev, exec, noexec, atime,
noatime, sync async, dirsync).  Filesystems are mounted with
'-onodev,nosuid' by default, which can only be overridden by a
privileged user.

These are FUSE specific mount options that can be specified for all
filesystems:

**default_permissions**

    By default FUSE doesn't check file access permissions, the
    filesystem is free to implement it's access policy or leave it to
    the underlying file access mechanism (e.g. in case of network
    filesystems).  This option enables permission checking, restricting
    access based on file mode.  This is option is usually useful
    together with the 'allow_other' mount option.

**allow_other**

    This option overrides the security measure restricting file access
    to the user mounting the filesystem.  So all users (including root)
    can access the files.  This option is by default only allowed to
    root, but this restriction can be removed with a configuration
    option described in the previous section.

**allow_root**

    This option is similar to 'allow_other' but file access is limited
    to the user mounting the filesystem and root.  This option and
    'allow_other' are mutually exclusive.

**kernel_cache**

    This option disables flushing the cache of the file contents on
    every open().  This should only be enabled on filesystems, where the
    file data is never changed externally (not through the mounted FUSE
    filesystem).  Thus it is not suitable for network filesystems and
    other "intermediate" filesystems.

    NOTE: if this option is not specified (and neither 'direct_io') data
    is still cached after the open(), so a read() system call will not
    always initiate a read operation.

**large_read**

    Issue large read requests.  This can improve performance for some
    filesystems, but can also degrade performance.  This option is only
    useful on 2.4.X kernels, as on 2.6 kernels requests size is
    automatically determined for optimum performance.

**direct_io**

    This option disables the use of page cache (file content cache) in
    the kernel for this filesystem.  This has several affects:

       * Each read() or write() system call will initiate one or more
         read or write operations, data will not be cached in the
         kernel.

       * The return value of the read() and write() system calls will
         correspond to the return values of the read and write
         operations.  This is useful for example if the file size is not
         known in advance (before reading it).

**max_read=N**

    With this option the maximum size of read operations can be set.
    The default is infinite.  Note that the size of read requests is
    limited anyway to 32 pages (which is 128kbyte on i386).

**hard_remove**

    The default behavior is that if an open file is deleted, the file is
    renamed to a hidden file (.fuse_hiddenXXX), and only removed when
    the file is finally released.  This relieves the filesystem
    implementation of having to deal with this problem.  This option
    disables the hiding behavior, and files are removed immediately in
    an unlink operation (or in a rename operation which overwrites an
    existing file).

    It is recommended that you not use the hard_remove option. When
    hard_remove is set, the following libc functions fail on unlinked
    files (returning errno of ENOENT):
      *  read()
      *  write()
      *  fsync()
      *  close()
      *  f*xattr()
      *  ftruncate()
      *  fstat()
      *  fchmod()
      *  fchown()

**debug**

    Turns on debug information printing by the library.

**fsname=NAME**

    Sets the filesystem name.  The default is the program name.

**use_ino**

    Honor the 'st_ino' field in getattr() and fill_dir().  This value is
    used to fill in the 'st_ino' field in the stat()/lstat()/fstat()
    functions and the 'd_ino' field in the readdir() function.  The
    filesystem does not have to guarantee uniqueness, however some
    applications rely on this value being unique for the whole
    filesystem.

**readdir_ino**

    If 'use_ino' option is not given, still try to fill in the 'd_ino'
    field in readdir().  If the name was previously looked up, and is
    still in the cache, the inode number found there will be used.
    Otherwise it will be set to '-1'.  If 'use_ino' option is given,
    this option is ignored.

**nonempty**

    Allows mounts over a non-empty file or directory.  By default these
    mounts are rejected (from version 2.3.1) to prevent accidental
    covering up of data, which could for example prevent automatic
    backup.

**umask=M**

    Override the permission bits in 'st_mode' set by the filesystem.
    The resulting permission bits are the ones missing from the given
    umask value.  The value is given in octal representation.

**uid=N**

    Override the 'st_uid' field set by the filesystem.

**gid=N**

    Override the 'st_gid' field set by the filesystem.

**blkdev**

    Mount a filesystem backed by a block device.  This is a privileged
    option.  The device must be specified with the 'fsname=NAME' option.


Reporting bugs
==============

Please send bug reports to the <zyd_com@126.com>.

