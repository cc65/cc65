#
# Makefile for GEOS lib
# for cc65
# 

%.o:	%.s
	@echo $<
	@$(AS) -o $@ $(AFLAGS) $<


S_OBJS	= get1stdirentry.o getnxtdirentry.o\
	  openrecordfile.o closerecordfile.o nextrecord.o previousrecord.o pointrecord.o\
	  deleterecord.o insertrecord.o appendrecord.o readrecord.o writerecord.o\
	  updaterecordfile.o\
	  findfile.o followchain.o getfhdrinfo.o readfile.o savefile.o freefile.o\
	  deletefile.o renamefile.o findftypes.o readbyte.o

all: $(S_OBJS)

clean:
	@rm -f *.~ $(S_OBJS) core