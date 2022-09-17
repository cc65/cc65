
.SILENT:

CC65=../../bin/cc65
CL65=../../bin/cl65
CA65=../../bin/ca65
SIM65=../../bin/sim65

OPT=

FILES=\
	quick.prg \
	\
	float-minimal.prg \
	float-minimal.bin \
	float-minimal.woz.bin \
	float-minimal.soft.bin \
	\
	float-basic.prg \
	float-basic.bin \
	float-basic.woz.bin \
	float-basic.soft.bin \
	\
	float-conv.prg \
	float-conv.bin \
	float-conv.woz.bin \
	float-conv.soft.bin \
	\
	float-cmp.prg \
	float-cmp.bin \
	float-cmp.woz.bin \
	float-cmp.soft.bin \
	\
	float-misc.prg \
	float-misc.bin \
	float-misc.woz.bin \
	float-misc.soft.bin

##############################################################################
HEADER=\
	include/_float.h \
	include/math.h

CBMRUNTIME=\
	cbmkernal/float.s \
	cbmkernal/ftoa.c \
	cbmkernal/ffloor.c \
	cbmkernal/cc65wrapper.s

SOFTFLOAT=\
	softfloat/softfloat.c \
	softfloat/ftostr.c \
	softfloat/cc65wrapper.s

IEEERUNTIME=\
	ieee754/feaxint.s \
	ieee754/feaxlong.s \
	ieee754/fbnegeax.s \
	ieee754/eaxufloat.s \
	ieee754/eaxfloat.s \
	ieee754/axufloat.s \
	ieee754/axfloat.s \
	ieee754/aufloat.s \
	ieee754/afloat.s \
	ieee754/ftosaddeax.s \
	ieee754/ftossubeax.s \
	ieee754/ftosdiveax.s \
	ieee754/ftosmuleax.s \
	ieee754/ftoseqeax.s \
	ieee754/ftosneeax.s \
	ieee754/ftosgteax.s \
	ieee754/ftoslteax.s \
	ieee754/ftosgeeax.s \
	ieee754/ftosleeax.s \
	ieee754/_ftostr.c

WOZRUNTIME=\
	woz/feaxint.s \
	woz/feaxlong.s \
	woz/fbnegeax.s \
	woz/eaxufloat.s \
	woz/eaxfloat.s \
	woz/axufloat.s \
	woz/axfloat.s \
	woz/aufloat.s \
	woz/afloat.s \
	woz/ftosaddeax.s \
	woz/ftossubeax.s \
	woz/ftosdiveax.s \
	woz/ftosmuleax.s \
	woz/ftoseqeax.s \
	woz/ftosneeax.s \
	woz/ftosgteax.s \
	woz/ftoslteax.s \
	woz/ftosgeeax.s \
	woz/ftosleeax.s \
	woz/_ftostr.c \
	woz/wozfp.s

##############################################################################
# The IEEE755 assembler implementation

all: $(FILES)

float-minimal.s: float-minimal.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.s float-minimal.c
float-minimal.bin: float-minimal.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-minimal.bin float-minimal.s $(IEEERUNTIME)

float-basic.s: float-basic.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic.s float-basic.c
float-basic.bin: float-basic.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic.bin float-basic.s $(IEEERUNTIME)

float-conv.s: float-conv.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-conv.s float-conv.c
float-conv.bin: float-conv.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-conv.bin float-conv.s $(IEEERUNTIME)

float-cmp.s: float-cmp.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-cmp.s float-cmp.c
float-cmp.bin: float-cmp.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-cmp.bin float-cmp.s $(IEEERUNTIME)

float-misc.s: float-misc.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-misc.s float-misc.c
float-misc.bin: float-misc.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-misc.bin float-misc.s $(IEEERUNTIME)

###############################################################################
# WOZ floats

float-minimal.woz.s: float-minimal.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.woz.s float-minimal.c
float-minimal.woz.bin: float-minimal.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-minimal.woz.bin float-minimal.woz.s $(WOZRUNTIME)

float-basic.woz.s: float-basic.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic.woz.s float-basic.c
float-basic.woz.bin: float-basic.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic.woz.bin float-basic.woz.s $(WOZRUNTIME)

float-conv.woz.s: float-conv.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-conv.woz.s float-conv.c
float-conv.woz.bin: float-conv.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-conv.woz.bin float-conv.woz.s $(WOZRUNTIME)

float-cmp.woz.s: float-cmp.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-cmp.woz.s float-cmp.c
float-cmp.woz.bin: float-cmp.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-cmp.woz.bin float-cmp.woz.s $(WOZRUNTIME)

float-misc.woz.s: float-misc.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-misc.woz.s float-misc.c
float-misc.woz.bin: float-misc.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-misc.woz.bin float-misc.woz.s $(WOZRUNTIME)

runwoz: float-minimal.woz.bin float-basic.woz.bin float-cmp.woz.bin
	$(SIM65) float-minimal.woz.bin
	$(SIM65) float-basic.woz.bin
	$(SIM65) float-cmp.woz.bin

###############################################################################
# Berkeley softfloat

softfloat.s:
	$(CL65) $(OPT) -S -t sim6502 -I ./include -o softfloat.s softfloat/softfloat.c

float-minimal.soft.s: float-minimal.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.soft.s float-minimal.c
float-minimal.soft.bin: float-minimal.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-minimal.soft.bin float-minimal.soft.s $(SOFTFLOAT)

float-basic.soft.s: float-basic.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic.soft.s float-basic.c
float-basic.soft.bin: float-basic.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic.soft.bin float-basic.soft.s $(SOFTFLOAT)

float-conv.soft.s: float-conv.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-conv.soft.s float-conv.c
float-conv.soft.bin: float-conv.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-conv.soft.bin float-conv.soft.s $(SOFTFLOAT)

float-cmp.soft.s: float-cmp.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-cmp.soft.s float-cmp.c
float-cmp.soft.bin: float-cmp.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-cmp.soft.bin float-cmp.soft.s $(SOFTFLOAT)

float-misc.soft.s: float-misc.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-misc.soft.s float-misc.c
float-misc.soft.bin: float-misc.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-misc.soft.bin float-misc.soft.s $(SOFTFLOAT)

quick.soft.s: quick.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o quick.soft.s quick.c
quick.soft.bin: quick.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o quick.soft.bin quick.soft.s $(SOFTFLOAT)

runsoft-minimal: float-minimal.soft.bin
	$(SIM65) float-minimal.soft.bin
runsoft-basic: float-basic.soft.bin
	$(SIM65) float-basic.soft.bin
runsoft-cmp: float-cmp.soft.bin
	$(SIM65) float-cmp.soft.bin
runsoft-conv: float-conv.soft.bin
	$(SIM65) float-conv.soft.bin
runsoft-misc: float-misc.soft.bin
	$(SIM65) float-misc.soft.bin

runsoft-quick: quick.soft.bin
	$(SIM65) quick.soft.bin
	
runsoft: float-minimal.soft.bin float-basic.soft.bin float-cmp.soft.bin float-conv.soft.bin float-misc.soft.bin
	$(SIM65) float-minimal.soft.bin
	$(SIM65) float-misc.soft.bin
	-$(SIM65) float-conv.soft.bin
	-$(SIM65) float-cmp.soft.bin
	-$(SIM65) float-basic.soft.bin

###############################################################################
# CBM kernal floats

float-minimal.c64.s: float-minimal.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-minimal.c64.s float-minimal.c
float-minimal.prg: float-minimal.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal -o float-minimal.prg float-minimal.c64.s $(CBMRUNTIME)
	
float-basic.c64.s: float-basic.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-basic.c64.s float-basic.c
float-basic.prg: float-basic.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-basic.prg float-basic.c64.s $(CBMRUNTIME)

float-conv.c64.s: float-conv.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-conv.c64.s float-conv.c
float-conv.prg: float-conv.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-conv.prg float-conv.c64.s $(CBMRUNTIME)

float-cmp.c64.s: float-cmp.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-cmp.c64.s float-cmp.c
float-cmp.prg: float-cmp.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-cmp.prg float-cmp.c64.s $(CBMRUNTIME)

float-misc.c64.s: float-misc.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-misc.c64.s float-misc.c
float-misc.prg: float-misc.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-misc.prg float-misc.c64.s $(CBMRUNTIME)

quick.c64.s: quick.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o quick.c64.s quick.c
quick.prg: quick.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal -o quick.prg quick.c64.s $(CBMRUNTIME)

runc64-quick:	quick.prg
	x64sc -autostartprgmode 1 quick.prg
###############################################################################
# test against GCC

gccminimal: float-minimal.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o float-minimal float-minimal.c gccstubs.c -lm
gccbasic: float-basic.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o float-basic float-basic.c gccstubs.c -lm
gccconv: float-conv.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o float-conv float-conv.c gccstubs.c -lm
gcccmp: float-cmp.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o float-cmp float-cmp.c gccstubs.c -lm
gccmisc: float-misc.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o float-misc float-misc.c gccstubs.c -lm
gccquick: quick.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o quick quick.c gccstubs.c -lm

rungcc-minimal: gccminimal
	./float-minimal
rungcc-basic: gccbasic
	./float-basic
rungcc-conv: gccconv
	./float-conv
rungcc-cmp: gcccmp
	./float-cmp
rungcc-misc: gccmisc
	./float-misc
rungcc-quick: gccquick
	./quick
	
rungcc: gccminimal gccconv gccbasic gcccmp
	./minimal
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

	$(RM) $(FILES)

	$(RM) softfloat.s

	$(RM) quick.bin
	$(RM) quick.c64.prg
	$(RM) quick.woz.bin
	$(RM) quick.soft.bin

	$(RM) quick.s
	$(RM) quick.c64.s
	$(RM) quick.woz.s
	$(RM) quick.soft.s

	$(RM) float-minimal.s
	$(RM) float-minimal.c64.s
	$(RM) float-minimal.woz.s
	$(RM) float-minimal.soft.s

	$(RM) float-basic.s
	$(RM) float-basic.c64.s
	$(RM) float-basic.woz.s
	$(RM) float-basic.soft.s

	$(RM) float-cmp.s
	$(RM) float-cmp.c64.s
	$(RM) float-cmp.woz.s
	$(RM) float-cmp.soft.s

	$(RM) float-conv.s
	$(RM) float-conv.c64.s
	$(RM) float-conv.woz.s
	$(RM) float-conv.soft.s
	
	$(RM) float-misc.s
	$(RM) float-misc.c64.s
	$(RM) float-misc.woz.s
	$(RM) float-misc.soft.s
	
	$(MAKE) -C softfloat clean
