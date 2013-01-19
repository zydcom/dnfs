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
**fisrt install fuse**

    ./configure
    make 
    make install
    modprobe fuse

You may also need to add '/usr/local/lib' to '/etc/ld.so.conf'  
then run ldconfig.

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


eporting bugs
==============

Please send bug reports to the <zyd_com@126.com>.

