
OPT=

FILES=\
	quick.prg \
	\
	float-minimal.prg \
	float-minimal.bin \
	float-minimal.woz.bin \
	\
	float-basic.prg \
	float-basic.bin \
	float-basic.woz.bin \
	\
	float-basic-cmp.prg \
	float-basic-cmp.bin \
	float-basic-cmp.woz.bin
	
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
	cc65 $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.s float-minimal.c
float-minimal.bin: float-minimal.s $(HEADER) $(IEEERUNTIME)
	cl65 $(OPT) -t sim6502 -I ./include -o float-minimal.bin float-minimal.s $(IEEERUNTIME)

float-basic.s: float-basic.c $(HEADER) $(IEEERUNTIME)
	cc65 $(OPT) -t sim6502 -I ./include --add-source -o float-basic.s float-basic.c
float-basic.bin: float-basic.s $(HEADER) $(IEEERUNTIME)
	cl65 $(OPT) -t sim6502 -I ./include -o float-basic.bin float-basic.s $(IEEERUNTIME)

float-basic-cmp.s: float-basic-cmp.c $(HEADER) $(IEEERUNTIME)
	cc65 $(OPT) -t sim6502 -I ./include --add-source -o float-basic-cmp.s float-basic-cmp.c
float-basic-cmp.bin: float-basic-cmp.s $(HEADER) $(IEEERUNTIME)
	cl65 $(OPT) -t sim6502 -I ./include -o float-basic-cmp.bin float-basic-cmp.s $(IEEERUNTIME)

###############################################################################
float-minimal.woz.s: float-minimal.c $(HEADER) $(WOZRUNTIME)
	cc65 $(OPT) -t sim6502 -I ./include --add-source -o float-minimal.woz.s float-minimal.c
float-minimal.woz.bin: float-minimal.woz.s $(HEADER) $(WOZRUNTIME)
	cl65 $(OPT) -t sim6502 -I ./include -o float-minimal.woz.bin float-minimal.woz.s $(WOZRUNTIME)

float-basic.woz.s: float-basic.c $(HEADER) $(WOZRUNTIME)
	cc65 $(OPT) -t sim6502 -I ./include --add-source -o float-basic.woz.s float-basic.c
float-basic.woz.bin: float-basic.woz.s $(HEADER) $(WOZRUNTIME)
	cl65 $(OPT) -t sim6502 -I ./include -o float-basic.woz.bin float-basic.woz.s $(WOZRUNTIME)

float-basic-cmp.woz.s: float-basic-cmp.c $(HEADER) $(WOZRUNTIME)
	cc65 $(OPT) -t sim6502 -I ./include --add-source -o float-basic-cmp.woz.s float-basic-cmp.c
float-basic-cmp.woz.bin: float-basic-cmp.woz.s $(HEADER) $(WOZRUNTIME)
	cl65 $(OPT) -t sim6502 -I ./include -o float-basic-cmp.woz.bin float-basic-cmp.woz.s $(WOZRUNTIME)

###############################################################################
float-minimal.c64.s: float-minimal.c $(HEADER) $(CBMRUNTIME)
	cc65 $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-minimal.c64.s float-minimal.c
float-minimal.prg: float-minimal.c64.s $(HEADER) $(CBMRUNTIME)
	cl65 $(OPT) -t c64 -I ./include -I ./cbmkernal -o float-minimal.prg float-minimal.c64.s $(CBMRUNTIME)
	
float-basic.c64.s: float-basic.c $(HEADER) $(CBMRUNTIME)
	cc65 $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-basic.c64.s float-basic.c
float-basic.prg: float-basic.c64.s $(HEADER) $(CBMRUNTIME)
	cl65 $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-basic.prg float-basic.c64.s $(CBMRUNTIME)

float-basic-cmp.c64.s: float-basic-cmp.c $(HEADER) $(CBMRUNTIME)
	cc65 $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o float-basic-cmp.c64.s float-basic-cmp.c
float-basic-cmp.prg: float-basic-cmp.c64.s $(HEADER) $(CBMRUNTIME)
	cl65 $(OPT) -t c64 -I ./include -I ./cbmkernal  -o float-basic-cmp.prg float-basic-cmp.c64.s $(CBMRUNTIME)

floattest.c64.s: floattest.c $(HEADER) $(CBMRUNTIME)
	cc65 $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o floattest.c64.s floattest.c
floattest.prg: floattest.c64.s $(HEADER) $(CBMRUNTIME)
	cl65 $(OPT) -t c64 -I ./include -I ./cbmkernal  -o floattest.prg floattest.c64.s $(CBMRUNTIME)

quick.c64.s: quick.c $(HEADER) $(CBMRUNTIME)
	cc65 $(OPT) -DCONIO -t c64 -I ./include -I ./cbmkernal --add-source -o quick.c64.s quick.c
quick.prg: quick.c64.s $(HEADER) $(CBMRUNTIME)
	cl65 $(OPT) -t c64 -I ./include -I ./cbmkernal -o quick.prg quick.c64.s $(CBMRUNTIME)
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
	sim65 float-minimal.bin
runbasic: float-basic.bin
	sim65 float-basic.bin
runbasiccmp: float-basic-cmp.bin
	sim65 float-basic-cmp.bin

run: runminimal runbasic runbasiccmp
	sim65 float-minimal.bin
	sim65 float-basic.bin
	sim65 float-basic-cmp.bin
	
runwoz: float-minimal.woz.bin float-basic.woz.bin float-basic-cmp.woz.bin
	sim65 float-minimal.woz.bin
	sim65 float-basic.woz.bin
	sim65 float-basic-cmp.woz.bin
	
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
