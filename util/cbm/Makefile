.SILENT:

PETCAT ?= petcat

CL65 ?= cl65

DEL = $(RM)

all: cbmcvt

cbmcvt: cbmcvt.c
	$(CC) -o cbmcvt cbmcvt.c

all-alphanumeric-petscii.bin: petscii.s
	$(CL65) -t none --asm-define CTRLCODES=0 --asm-define GFXCODES=0 -o all-alphanumeric-petscii.bin petscii.s
all-printable-petscii.bin: petscii.s
	$(CL65) -t none --asm-define CTRLCODES=0 --asm-define GFXCODES=1 -o all-printable-petscii.bin petscii.s
all-petscii.bin: petscii.s
	$(CL65) -t none --asm-define CTRLCODES=1 --asm-define GFXCODES=1 -o all-petscii.bin petscii.s

all-alphanumeric-iso8859.bin: iso8859.s
	$(CL65) -t none --asm-define CTRLCODES=0 --asm-define NONASCII=0 -o all-alphanumeric-iso8859.bin iso8859.s
all-printable-iso8859.bin: iso8859.s
	$(CL65) -t none --asm-define CTRLCODES=0 --asm-define NONASCII=1 -o all-printable-iso8859.bin iso8859.s
all-iso8859.bin: iso8859.s
	$(CL65) -t none --asm-define CTRLCODES=1 --asm-define NONASCII=1 -o all-iso8859.bin iso8859.s

testfiles: all-alphanumeric-petscii.bin all-printable-petscii.bin all-petscii.bin\
	all-alphanumeric-iso8859.bin all-printable-iso8859.bin all-iso8859.bin
	
test: test1 test2
	
#verify that petscii -> ascii -> petscii produces consistant and reversible results
test1: ./cbmcvt all-alphanumeric-petscii.bin
# convert text from petscii to ascii
	./cbmcvt < all-alphanumeric-petscii.bin > toasc.out
# convert text from ascii to petscii
	./cbmcvt -p < toasc.out > topet.out
	diff  -q -s all-alphanumeric-petscii.bin topet.out

#compare petscii -> ascii with what petcat does
test1a: ./cbmcvt all-alphanumeric-petscii.bin
# convert text from petscii to ascii
	$(PETCAT) -text -nh -o toasc1.out all-alphanumeric-petscii.bin
# convert text from petscii to ascii
	./cbmcvt < all-alphanumeric-petscii.bin > toasc2.out
	diff  -q -s toasc1.out toasc2.out
	
#verify that petscii -> ascii -> petscii produces consistant and reversible results
test2: ./cbmcvt all-printable-petscii.bin
# convert text from petscii to ascii
	./cbmcvt < all-printable-petscii.bin > toasc.out
# convert text from ascii to petscii
	./cbmcvt -p < toasc.out > topet.out
	-diff  -q -s all-printable-petscii.bin topet.out
	
#verify that petscii -> ascii -> petscii produces consistant and reversible results
test3: ./cbmcvt all-petscii.bin
# convert text from petscii to ascii
	./cbmcvt < all-petscii.bin > toasc.out
# convert text from ascii to petscii
	./cbmcvt -p < toasc.out > topet.out
	diff  -q -s all-petscii.bin topet.out

#verify that iso8859 -> petscii -> iso8859 produces consistant and reversible results
test4: ./cbmcvt all-alphanumeric-iso8859.bin
# convert text from iso8859 to petscii
	./cbmcvt -p < all-alphanumeric-iso8859.bin > topet.out
# convert text from petscii to iso8859
	./cbmcvt < topet.out > toasc.out
	diff  -q -s all-alphanumeric-iso8859.bin toasc.out

#compare iso8859 -> petscii with what petcat does
test4a: ./cbmcvt all-alphanumeric-iso8859.bin
# convert text from iso8859 to petscii
	$(PETCAT) -text -nh -w2 -o topet1.out all-alphanumeric-iso8859.bin
# convert text from iso8859 to petscii
	./cbmcvt -p < all-alphanumeric-iso8859.bin > topet2.out
	diff  -q -s topet1.out topet2.out
	
#verify that iso8859 -> petscii -> iso8859 produces consistant and reversible results
test5: ./cbmcvt all-printable-iso8859.bin
# convert text from iso8859 to petscii
	./cbmcvt -p < all-printable-iso8859.bin > topet.out
# convert text from petscii to iso8859
	./cbmcvt < topet.out > toasc.out
	-diff  -q -s all-printable-iso8859.bin toasc.out
	
#verify that iso8859 -> petscii -> iso8859 produces consistant and reversible results
test6: ./cbmcvt all-iso8859.bin
# convert text from iso8859 to petscii
	./cbmcvt -p < all-iso8859.bin > topet.out
# convert text from petscii to iso8859
	./cbmcvt < topet.out > toasc.out
	diff  -q -s all-iso8859.bin toasc.out

clean:
	$(DEL) *.bin
	$(DEL) *.out
	$(DEL) cbmcvt
