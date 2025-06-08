CC=gcc
CFLAGS=-g
IFLAGS=-I../common

SRCS=\
    ../common/check.c \
    ../common/cmdline.c \
    ../common/chartype.c \
    ../common/abend.c \
    ../common/xmalloc.c \
    ../common/fname.c \
    ../common/coll.c \
    dbginfo.c \
    dbgsh.c

../../wrk/dbginfo/dbgsh:
	mkdir -p ../../wrk/dbginfo
	$(CC) $(CFLAGS) $(IFLAGS) $(SRCS) -o $@

clean:
	rm -rf ../../wrk/dbginfo
