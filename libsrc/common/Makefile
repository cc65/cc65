#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS =	_afailed.o	\
		_hextab.o	\
		_scanf.o	\
		abort.o		\
                asctime.o       \
		bsearch.o	\
		errormsg.o	\
		fdopen.o	\
		fgetc.o		\
		fgetpos.o	\
		fgets.o		\
		fputc.o		\
		fputs.o		\
		freopen.o	\
		fseek.o		\
		fsetpos.o	\
		ftell.o		\
		getchar.o	\
		gets.o		\
                gmtime.o        \
		locale.o	\
                localtime.o     \
                mktime.o        \
		perror.o	\
		putchar.o	\
		puts.o 		\
		qsort.o		\
		realloc.o	\
		rewind.o	\
		sscanf.o	\
                strftime.o      \
		strxfrm.o	\
		strtok.o	\
                timezone.o      \
		vprintf.o       \
                vsscanf.o


S_OBJS = 	_fdesc.o 	\
	 	_file.o  	\
		_fopen.o	\
	 	_heap.o	 	\
       	       	_heapadd.o 	\
	 	_oserror.o	\
		_printf.o	\
		_swap.o		\
	 	_sys.o		\
		abs.o		\
		atexit.o	\
		atoi.o		\
		calloc.o	\
		copydata.o	\
		cprintf.o	\
                ctime.o         \
                divt.o          \
		errno.o		\
		fclose.o	\
		fmisc.o 	\
		fopen.o		\
		fprintf.o	\
		fread.o		\
		free.o		\
		fwrite.o	\
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
		modfree.o       \
		modload.o       \
		printf.o      	\
	 	rand.o	      	\
	 	setjmp.o      	\
		sprintf.o	\
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
		vcprintf.o	\
		vfprintf.o	\
		vsprintf.o	\
		zerobss.o


all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)

