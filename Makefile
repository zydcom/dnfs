LIB = `pkg-config fuse --libs`
CFLAGS = `pkg-config fuse --cflags` -Wall

TAR = dnfs-mount dnfs-server dnfs-node

all: $(TAR)

#build dnfs-mount
dnfs-mount: cs_clnt.o cs_xdr.o co_xdr.o co_clnt.o \
client.o fs.o cache.o common.o 
	cc -o $@ $^ $(LIB) -g
	
client.o: client.c client.h
	cc -c $<

fs.o: fs.c
	cc -c $< $(CFLAGS)

cache.o: cache.c cache.h
	cc -c $<

#build dnfs-server
dnfs-server: cs_svc.o cs_xdr.o \
server.o wrapper.o manage.o common.o 
	cc -o $@ $^ -l pthread -g -lcrypt

server.o: server.c server.h
	cc -c $<

manage.o: manage.c server.h
	cc -c $<
	
#build dnfs-node
dnfs-node: co_xdr.o co_clnt.o co_svc.o cs_clnt.o cs_xdr.o \
storage.o wrapper.o heart.o common.o
	cc -o $@ $^ -lpthread 
	
heart.o: heart.c storage.h
	cc -c $<

storage.o: storage.c storage.h
	cc -c $<

#common targets
common.o: common.c common.h
	cc -c $<

wrapper.o: wrapper.c wrapper.h
	cc -c $<

cs_clnt.o: cs_clnt.c cs.h
	cc -c $<
	
cs_xdr.o: cs_xdr.c cs.h
	cc -c $<

cs_svc.o: cs_svc.c cs.h
	cc -c $<
	
co_clnt.o: co_clnt.c co.h
	cc -c $<
	
co_xdr.o: co_xdr.c co.h
	cc -c $<

co_svc.o: co_svc.c co.h
	cc -c $<

#make install
install:
	cp TAR /usr/bin

clean:
	rm *.o
	rm $(TAR)
