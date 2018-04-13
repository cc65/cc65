CL ?= cl65

all: c64-scpu-test.prg c128-scpu-test.prg

c64-scpu-test.prg: c64-c128-scpu-test.c
	$(CL) -t c64 c64-c128-scpu-test.c -o c64-scpu-test.prg

c128-scpu-test.prg: c64-c128-scpu-test.c
	$(CL) -t c128 c64-c128-scpu-test.c -o c128-scpu-test.prg
