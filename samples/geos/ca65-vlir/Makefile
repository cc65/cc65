all: vlir0.s vlir1.s vlir2.s cvthead.grc
	grc cvthead.grc
	ca65 cvthead.s
	ca65 vlir0.s
	ca65 vlir1.s
	ca65 vlir2.s
	ld65 -C cvthead.cfg cvthead.o vlir0.o vlir1.o vlir2.o
	vlink out.cvt vlir-head vlir0 vlir1 vlir2

.PHONY: clean

clean:
	-rm *.o vlir-head vlir0 vlir1 vlir2 cvthead.s cvthead.cfg out.cvt

love:
	@echo "Not war, eh?"
