
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
	float-basic-cmp.prg \
	float-basic-cmp.bin \
	float-basic-cmp.woz.bin \
	float-basic-cmp.soft.bin
	
#	
#	floattest.prg

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
	
all: $(FILES)

float-minimal.s: float-minimal.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.s float-minimal.c
float-minimal.bin: float-minimal.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-minimal.bin float-minimal.s $(IEEERUNTIME)

float-basic.s: float-basic.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic.s float-basic.c
float-basic.bin: float-basic.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic.bin float-basic.s $(IEEERUNTIME)

float-basic-cmp.s: float-basic-cmp.c $(HEADER) $(IEEERUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic-cmp.s float-basic-cmp.c
float-basic-cmp.bin: float-basic-cmp.s $(HEADER) $(IEEERUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic-cmp.bin float-basic-cmp.s $(IEEERUNTIME)

###############################################################################
float-minimal.woz.s: float-minimal.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.woz.s float-minimal.c
float-minimal.woz.bin: float-minimal.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-minimal.woz.bin float-minimal.woz.s $(WOZRUNTIME)

float-basic.woz.s: float-basic.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic.woz.s float-basic.c
float-basic.woz.bin: float-basic.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic.woz.bin float-basic.woz.s $(WOZRUNTIME)

float-basic-cmp.woz.s: float-basic-cmp.c $(HEADER) $(WOZRUNTIME)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic-cmp.woz.s float-basic-cmp.c
float-basic-cmp.woz.bin: float-basic-cmp.woz.s $(HEADER) $(WOZRUNTIME)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic-cmp.woz.bin float-basic-cmp.woz.s $(WOZRUNTIME)

###############################################################################
float-minimal.soft.s: float-minimal.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.soft.s float-minimal.c
float-minimal.soft.bin: float-minimal.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-minimal.soft.bin float-minimal.soft.s $(SOFTFLOAT)

float-basic.soft.s: float-basic.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic.soft.s float-basic.c
float-basic.soft.bin: float-basic.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic.soft.bin float-basic.soft.s $(SOFTFLOAT)

float-basic-cmp.soft.s: float-basic-cmp.c $(HEADER) $(SOFTFLOAT)
	$(CC65) $(OPT) -t sim6502 -I ./include --add-source -o float-basic-cmp.soft.s float-basic-cmp.c
float-basic-cmp.soft.bin: float-basic-cmp.soft.s $(HEADER) $(SOFTFLOAT)
	$(CL65) $(OPT) -t sim6502 -I ./include -o float-basic-cmp.soft.bin float-basic-cmp.soft.s $(SOFTFLOAT)

###############################################################################
float-minimal.c64.s: float-minimal.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-minimal.c64.s float-minimal.c
float-minimal.prg: float-minimal.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal -o float-minimal.prg float-minimal.c64.s $(CBMRUNTIME)
	
float-basic.c64.s: float-basic.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-basic.c64.s float-basic.c
float-basic.prg: float-basic.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-basic.prg float-basic.c64.s $(CBMRUNTIME)

float-basic-cmp.c64.s: float-basic-cmp.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-basic-cmp.c64.s float-basic-cmp.c
float-basic-cmp.prg: float-basic-cmp.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-basic-cmp.prg float-basic-cmp.c64.s $(CBMRUNTIME)

floattest.c64.s: floattest.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o floattest.c64.s floattest.c
floattest.prg: floattest.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal  -o floattest.prg floattest.c64.s $(CBMRUNTIME)

quick.c64.s: quick.c $(HEADER) $(CBMRUNTIME)
	$(CC65) $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o quick.c64.s quick.c
quick.prg: quick.c64.s $(HEADER) $(CBMRUNTIME)
	$(CL65) $(OPT) -t c64 -I ./include -I ./cbmkernal -o quick.prg quick.c64.s $(CBMRUNTIME)
runquick:	quick.prg
	x64sc -autostartprgmode 1 quick.prg
###############################################################################
gccminimal: float-minimal.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o minimal float-minimal.c gccstubs.c -lm
gccbasic: float-basic.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o basic float-basic.c gccstubs.c -lm
gccbasiccmp: float-basic-cmp.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o basiccmp float-basic-cmp.c gccstubs.c -lm
gccfloattest: floattest.c gccstubs.c
	gcc -I ./include "-D__fastcall__= " -o floattest floattest.c gccstubs.c -lm

rungcc: gccminimal gccbasic gccbasiccmp gccfloattest
	./minimal
	./basic
	./basiccmp
	./floattest

###############################################################################
	
runminimal: float-minimal.bin
	$(SIM65) float-minimal.bin
runbasic: float-basic.bin
	$(SIM65) float-basic.bin
runbasiccmp: float-basic-cmp.bin
	$(SIM65) float-basic-cmp.bin

run: runminimal runbasic runbasiccmp
	$(SIM65) float-minimal.bin
	$(SIM65) float-basic.bin
	$(SIM65) float-basic-cmp.bin
	
runwoz: float-minimal.woz.bin float-basic.woz.bin float-basic-cmp.woz.bin
	$(SIM65) float-minimal.woz.bin
	$(SIM65) float-basic.woz.bin
	$(SIM65) float-basic-cmp.woz.bin
	
runsoft-minimal: float-minimal.soft.bin
	$(SIM65) float-minimal.soft.bin
runsoft-basic: float-basic.soft.bin
	$(SIM65) float-basic.soft.bin
runsoft-cmp: float-basic-cmp.soft.bin
	$(SIM65) float-basic-cmp.soft.bin
	
runsoft: float-minimal.soft.bin float-basic.soft.bin float-basic-cmp.soft.bin
	$(SIM65) float-minimal.soft.bin
	$(SIM65) float-basic.soft.bin
	$(SIM65) float-basic-cmp.soft.bin
	
###############################################################################

clean:
	$(RM) ./minimal
	$(RM) ./basic
	$(RM) ./basiccmp
	$(RM) ./floattest

	$(RM) $(FILES)

	$(RM) quick.c64.s

	$(RM) float-minimal.s
	$(RM) float-minimal.c64.s
	$(RM) float-minimal.woz.s

	$(RM) float-basic.s
	$(RM) float-basic.c64.s
	$(RM) float-basic.woz.s

	$(RM) float-basic-cmp.s
	$(RM) float-basic-cmp.c64.s
	$(RM) float-basic-cmp.woz.s
