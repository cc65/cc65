# -*- makefile -*-
#
# makefile for CC65's common library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Programs and flags

SYS	= none

AS	= ../../src/ca65/ca65
CC	= ../../src/cc65/cc65
LD	= ../../src/ld65/ld65

AFLAGS	= -t $(SYS) -I../../asminc
CFLAGS	= -Osir -g -T -t $(SYS) --forget-inc-paths -I . -I ../../include

#--------------------------------------------------------------------------
# Rules

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Rules to help us see what code the compiler and assembler make.

%.s :	%.c
	@$(CC) $(CFLAGS) -S $<

%.lst :	%.s
	@$(AS) $(AFLAGS) -l -o /dev/null $<

#--------------------------------------------------------------------------
# Object files

# From C source-files
C_OBJS =	_afailed.o	        \
                _aligned_malloc.o       \
		_hextab.o	        \
                _poserror.o             \
		_scanf.o	        \
		abort.o		        \
                asctime.o               \
		bsearch.o	        \
		errormsg.o	        \
		fdopen.o	        \
		fgetc.o		        \
		fgetpos.o	        \
		fgets.o		        \
		fputc.o		        \
		fputs.o		        \
		freopen.o	        \
		fseek.o		        \
		fsetpos.o	        \
		ftell.o		        \
		getchar.o	        \
		gets.o		        \
                gmtime.o                \
		locale.o	        \
                localtime.o             \
                mktime.o                \
		perror.o	        \
		puts.o 		        \
		qsort.o		        \
		realloc.o	        \
		rewind.o	        \
		sleep.o		        \
                strftime.o              \
		strxfrm.o	        \
		strtok.o	        \
                system.o                \
                timezone.o

# From assembly source-files
S_OBJS = 	_cwd.o          \
                _fdesc.o 	\
	 	_file.o  	\
		_fopen.o	\
	 	_heap.o	 	\
       	       	_heapadd.o 	\
                _heapblocksize.o\
                _heapmaxavail.o \
                _heapmemavail.o \
	 	_oserror.o	\
		_printf.o 	\
                _seterrno.o     \
		_swap.o	  	\
	 	_sys.o	  	\
		abs.o	  	\
		atexit.o  	\
		atoi.o	  	\
		calloc.o  	\
                chdir.o         \
		copydata.o	\
                creat.o         \
                ctime.o         \
                divt.o          \
		errno.o		\
		fclose.o	\
		fmisc.o 	\
		fopen.o		\
		fprintf.o	\
		fread.o		\
		free.o		\
                fscanf.o        \
		fwrite.o	\
		getcpu.o	\
                getcwd.o        \
		isalnum.o	\
		isalpha.o	\
		isblank.o	\
		iscntrl.o	\
		isdigit.o 	\
		isgraph.o	\
		islower.o	\
	 	isprint.o	\
		ispunct.o	\
		isspace.o	\
		isupper.o	\
		isxdigit.o	\
		itoa.o		\
		labs.o		\
	 	longjmp.o	\
	 	ltoa.o	    	\
		malloc.o	\
		maperrno.o  	\
		memchr.o    	\
		memcmp.o    	\
	 	memcpy.o      	\
                memmove.o       \
	 	memset.o      	\
		modfree.o       \
		modload.o       \
                oserrcheck.o    \
		printf.o      	\
		putchar.o	\
	 	rand.o	      	\
                raise.o         \
                remove.o        \
                rename.o        \
                scanf.o         \
	 	setjmp.o      	\
                signal.o        \
                sigtable.o      \
		sprintf.o	\
		sscanf.o	\
		strcat.o    	\
		strchr.o	\
		strcmp.o	\
		strcoll.o	\
		strcpy.o	\
		strcspn.o	\
		strdup.o	\
		strerror.o	\
		stricmp.o	\
		strlen.o	\
		strlower.o	\
		strncat.o	\
		strncmp.o	\
		strncpy.o	\
                stroserr.o      \
		strpbrk.o	\
		strrchr.o	\
		strspn.o	\
		strstr.o	\
		strupper.o	\
                time.o          \
		tolower.o	\
		toupper.o	\
                uname.o         \
                ungetc.o        \
                unlink.o        \
                utscopy.o       \
		vfprintf.o	\
                vfscanf.o       \
		vprintf.o       \
                vscanf.o        \
		vsprintf.o	\
                vsscanf.o       \
		zerobss.o


#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@$(RM) *~ *.lst
	@$(RM) $(C_OBJS:.o=.s)
	@$(RM) $(C_OBJS)
	@$(RM) $(S_OBJS)

zap:	clean

