#
# Makefile for CC65.COM.
#

.SUFFIXES: .o .obj .m65 .c

.c.m65:
	@echo $<
	@cc65 -I../lib65/ -O -t4 $<

.m65.obj:
	../ra65/ra65 -o $@ ../lib65/ace/global.m65 $<

C_SRCS = code-gen.c error.c expr1.c expr2.c expr3.c function.c mem.c loop.c\
	 globlvar.c io.c scanner.c main.c optimize.c preproc.c\
	 stmt.c symtab.c util.c declare.c

H_SRCS = cc65.h scanner.h error.h mem.h optimize.h code-gen.h function.h\
         preproc.h util.h symtab.h io.h ctrans.h stmt.h declare.h loop.h\
	 expr.h

M65_FILES = ccmisc.m65 extra.m65 rtextra.m65

OBJS = code-gen.obj error.obj expr1.obj expr2.obj expr3.obj function.obj \
	globlvar.obj io.obj scanner.obj main.obj\
	optimize.obj preproc.obj stmt.obj symtab.obj declare.obj loop.obj\
	ccmisc.obj extra.obj rtextra.obj ctrans.obj mem.obj util.obj

cc65.com: $(OBJS)
	@../ra65/link65 -t4 -m -o cc65.com ../lib65/ace/crt0.obj $(OBJS)\
	../lib65/ace.olb

.PRECIOUS:	$(C_SRCS:.c=.m65)


$(OBJS) : $(H_SRCS)

clean :
	rm -f $(OBJS)
	rm -f $(C_SRCS:.c=.m65)
