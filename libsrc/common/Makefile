#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS = fclose.o fgets.o calloc.o _fopen.o\
	 fputs.o fread.o fwrite.o gets.o realloc.o bsearch.o strxfrm.o\
	 _hextab.o fdopen.o strtok.o\
	 _afailed.o fopen.o fgetc.o fputc.o puts.o gets.o perror.o getchar.o\
	 vprintf.o abort.o qsort.o putchar.o\
	 errormsg.o vcprintf.o freopen.o locale.o fsetpos.o\
	 fgetpos.o rewind.o fseek.o ftell.o

S_OBJS = 	_fdesc.o	\
	 	_file.o 	\
	 	_hadd.o		\
	 	_heap.o		\
	 	_oserror.o	\
		_printf.o	\
	 	_stksize.o	\
		_swap.o		\
	 	_sys.o		\
		abs.o		\
		atexit.o	\
		atoi.o		\
		copydata.o	\
		cprintf.o	\
		errno.o		\
		fmisc.o 	\
		fprintf.o	\
		free.o		\
		getcpu.o	\
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
	 	memset.o      	\
		printf.o      	\
	 	rand.o	      	\
	 	setjmp.o      	\
		sprintf.o	\
		stkcheck.o  	\
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
		strpbrk.o	\
		strrchr.o	\
		strspn.o	\
		strstr.o	\
		strupper.o	\
		tolower.o	\
		toupper.o	\
		vfprintf.o	\
		vsprintf.o	\
		zerobss.o


all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)

