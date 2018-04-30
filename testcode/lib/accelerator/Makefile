CL ?= cl65

all:	c64-scpu-test.prg c128-scpu-test.prg c64dtv-test.prg \
	c64-c128-test.prg c128-test.prg chameleon-test.prg \
	c65-test.prg turbomaster-test.prg

c64-scpu-test.prg: c64-c128-scpu-test.c
	$(CL) -t c64 c64-c128-scpu-test.c -o c64-scpu-test.prg

c128-scpu-test.prg: c64-c128-scpu-test.c
	$(CL) -t c128 c64-c128-scpu-test.c -o c128-scpu-test.prg

c64dtv-test.prg: c64dtv-test.c
	$(CL) -t c64 c64dtv-test.c -o c64dtv-test.prg

c64-c128-test.prg: c64-c128-test.c
	$(CL) -t c64 c64-c128-test.c -o c64-c128-test.prg

c128-test.prg: c64-c128-test.c
	$(CL) -t c128 c64-c128-test.c -o c128-test.prg

chameleon-test.prg: chameleon-test.c
	$(CL) -t c64 chameleon-test.c -o chameleon-test.prg

c65-test.prg: c65-test.c
	$(CL) -t c64 c65-test.c -o c65-test.prg

turbomaster-test.prg: turbomaster-test.c
	$(CL) -t c64 turbomaster-test.c -o turbomaster-test.prg
