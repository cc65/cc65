#
# Makefile for GEOS lib
# for cc65
#

%.o:	%.s
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= blkalloc.o calcblksfree.o changediskdevice.o chkdkgeos.o enterturbo.o exitturbo.o\
	  findbambit.o freeblock.o getblock.o getdirhead.o getptrcurdknm.o newdisk.o\
	  nxtblkalloc.o opendisk.o purgeturbo.o putblock.o putdirhead.o readblock.o\
	  readbuff.o setnextfree.o setgeosdisk.o writeblock.o writebuff.o verwriteblock.o\
	  gettrse.o setoserror.o\
	  dio_openclose.o dio_cts.o dio_stc.o dio_read.o dio_write.o dio_writev.o\
	  dio_params.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core
