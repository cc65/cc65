#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

%.o:   	%.c
	@echo $<
	@$(CC) $(CFLAGS) $<
	@$(AS) -g -o $@ $(AFLAGS) $(*).s

%.o:	%.s
	@echo $<
	@$(AS) -g -o $@ $(AFLAGS) $<

C_OBJS = fclose.o fgets.o fprintf.o strdup.o calloc.o _fopen.o\
      	 fputs.o fread.o fwrite.o gets.o realloc.o bsearch.o strxfrm.o\
      	 printf.o _hextab.o malloc.o free.o vfprintf.o fdopen.o strtok.o\
    	 _afailed.o fopen.o fgetc.o fputc.o puts.o gets.o perror.o getchar.o\
    	 _printf.o vprintf.o vsprintf.o sprintf.o abort.o qsort.o putchar.o\
    	 errormsg.o _hadd.o cprintf.o vcprintf.o freopen.o locale.o

S_OBJS = isalpha.o isdigit.o _file.o fmisc.o strlower.o strchr.o tolower.o\
	 toupper.o errno.o strcpy.o strlen.o strcat.o strcmp.o itoa.o\
	 strupper.o isalpha.o isalnum.o isgraph.o islower.o isupper.o\
       	 isprint.o ispunct.o isspace.o isxdigit.o isblank.o strrchr.o\
	 _stksize.o _heap.o stricmp.o strncmp.o strncpy.o atoi.o setjmp.o\
	 longjmp.o rand.o atexit.o memset.o memcpy.o memchr.o memcmp.o\
	 ltoa.o strcspn.o strncat.o strpbrk.o strspn.o abs.o labs.o jmpvec.o\
	 _fdesc.o stkcheck.o zerobss.o copydata.o _swap.o strstr.o strcoll.o\
	 _sys.o getcpu.o _oserror.o strerror.o

all:  	$(C_OBJS) $(S_OBJS)

clean:
	@rm -f *~
	@rm -f $(C_OBJS:.o=.s)
	@rm -f $(C_OBJS)
	@rm -f $(S_OBJS)
