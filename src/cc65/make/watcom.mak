#
# CC65 Makefile for the Watcom compiler (using GNU make)
#

# ------------------------------------------------------------------------------
# Generic stuff

AR	= WLIB
LD	= WLINK
LNKCFG  = ld.tmp

# --------------------- OS2 ---------------------
ifeq ($(TARGET),OS2)
SYSTEM  = os2v2
CC      = WCC386
CFLAGS  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2
endif

# -------------------- DOS4G --------------------
ifeq ($(TARGET),DOS32)
SYSTEM  = dos4g
CC      = WCC386
CFLAGS  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2
endif

# --------------------- NT ----------------------
ifeq ($(TARGET),NT)
SYSTEM  = nt
CC      = WCC386
CFLAGS  = -bt=$(TARGET) -d1 -onatx -zp4 -5 -zq -w2
endif

# Add the include dir
CFLAGS  += -i=..\common

# ------------------------------------------------------------------------------
# Implicit rules

%.obj:  %.c
	$(CC) $(CFLAGS) $^


# ------------------------------------------------------------------------------
# All library OBJ files

OBJS =	anonname.obj	\
     	asmcode.obj	\
	asmlabel.obj	\
	asmstmt.obj	\
        assignment.obj  \
	casenode.obj	\
	codeent.obj	\
	codegen.obj	\
	codelab.obj	\
	codeinfo.obj	\
	codeopt.obj	\
	codeseg.obj	\
	compile.obj	\
	coptadd.obj	\
	coptc02.obj	\
	coptcmp.obj	\
	coptind.obj	\
	coptneg.obj	\
	coptpush.obj	\
        coptsize.obj    \
	coptstop.obj	\
        coptstore.obj   \
	coptsub.obj	\
	copttest.obj	\
	cpu.obj		\
	dataseg.obj	\
	datatype.obj	\
	declare.obj	\
	declattr.obj	\
	error.obj	\
	expr.obj 	\
        exprdesc.obj    \
	exprheap.obj	\
	exprnode.obj	\
	funcdesc.obj	\
	function.obj	\
	global.obj	\
	goto.obj	\
        hexval.obj      \
	ident.obj	\
	incpath.obj	\
	input.obj	\
	lineinfo.obj	\
	litpool.obj	\
	locals.obj	\
	loop.obj 	\
	macrotab.obj	\
	main.obj	\
	opcodes.obj	\
	preproc.obj	\
	pragma.obj	\
	reginfo.obj	\
    	scanner.obj	\
        scanstrbuf.obj  \
	segments.obj	\
    	stdfunc.obj	\
    	stmt.obj	\
	swstmt.obj	\
    	symentry.obj	\
    	symtab.obj	\
	textseg.obj	\
        typecast.obj    \
    	typecmp.obj	\
	util.obj

LIBS = ..\common\common.lib

# ------------------------------------------------------------------------------
# Main targets

all:		cc65

cc65:		cc65.exe


# ------------------------------------------------------------------------------
# Other targets


cc65.exe:	$(OBJS) $(LIBS)
	@echo DEBUG ALL > $(LNKCFG)
	@echo OPTION QUIET >> $(LNKCFG)
	@echo NAME $@ >> $(LNKCFG)
	@for %%i in ($(OBJS)) do echo FILE %%i >> $(LNKCFG)
	@for %%i in ($(LIBS)) do echo LIBRARY %%i >> $(LNKCFG)
	$(LD) system $(SYSTEM) @$(LNKCFG)
	@rm $(LNKCFG)

clean:
	@if exist *.obj del *.obj
	@if exist cc65.exe del cc65.exe

strip:
	@-wstrip cc65.exe


