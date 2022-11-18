
.SILENT:

CC65=../../bin/cc65
CL65=../../bin/cl65
CA65=../../bin/ca65
SIM65=../../bin/sim65

OPT=-O

TDIR=../val

all: quick

##############################################################################

quick.bin: quick.c
	$(CC65) $(OPT) -t sim6502 --add-source -o quick.s quick.c
#	gdb --args $(CC65) $(OPT) -t sim6502 --add-source -o quick.s quick.c
	$(CL65) $(OPT) -t sim6502 -o quick.bin quick.s

.PHONY: quick
	
quick: quick.bin
	$(SIM65) quick.bin

quick.c64.s: quick.c
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o quick.c64.s quick.c
quick.prg: quick.c64.s
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal -o quick.prg quick.c64.s

runc64-quick:	quick.prg
	x64sc -autostartprgmode 1 quick.prg
###############################################################################
# test against GCC

COPT=-m32 -mpc32

gccminimal: $(TDIR)/float-minimal.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-minimal $(TDIR)/float-minimal.c gccstubs.c -lm
gccbasic-const-const: $(TDIR)/float-basic-const-const.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-basic-const-const $(TDIR)/float-basic-const-const.c gccstubs.c -lm
gccbasic-var-const: $(TDIR)/float-basic-var-const.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-basic-var-const $(TDIR)/float-basic-var-const.c gccstubs.c -lm
gccbasic-const-var: $(TDIR)/float-basic-const-var.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-basic-const-var $(TDIR)/float-basic-const-var.c gccstubs.c -lm
gccbasic-var-var: $(TDIR)/float-basic-var-var.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-basic-var-var $(TDIR)/float-basic-var-var.c gccstubs.c -lm
gccconv: $(TDIR)/float-conv.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-conv $(TDIR)/float-conv.c gccstubs.c -lm
gcccmp: $(TDIR)/float-cmp.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-cmp $(TDIR)/float-cmp.c gccstubs.c -lm
gccmisc: $(TDIR)/float-misc.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o float-misc $(TDIR)/float-misc.c gccstubs.c -lm
gccquick: quick.c gccstubs.c
	gcc $(COPT) -I ./ "-D__fastcall__= " -o quick quick.c gccstubs.c -lm

rungcc-minimal: gccminimal
	./float-minimal
rungcc-basic: gccbasic-const-const gccbasic-var-const gccbasic-const-var gccbasic-var-var
	./float-basic-const-const
	./float-basic-var-const
	./float-basic-const-var
	./float-basic-var-var
rungcc-conv: gccconv
	./float-conv
rungcc-cmp: gcccmp
	./float-cmp
rungcc-misc: gccmisc
	./float-misc
rungcc-quick: gccquick
	./quick
	
rungcc: gccminimal gccconv gccbasic gcccmp gccmisc gccquick
	./float-minimal
	./float-basic
	./float-conv
	./float-cmp
	./float-misc
	
###############################################################################

clean:
	$(RM) ./float-minimal
	$(RM) ./float-basic
	$(RM) ./float-cmp
	$(RM) ./float-conv
	$(RM) ./float-misc
	$(RM) ./quick
	$(RM) ./quick.bin
	$(RM) ./quick.s

	$(RM) quick.prg
	$(RM) quick.c64.s
	$(RM) quick.s
