
# Just the usual way to find out if we're
# using cmd.exe to execute make rules.
ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE
  NULLDEV = nul:
  DEL = -del /f
  RMDIR = rmdir /s /q
else
  NULLDEV = /dev/null
  DEL = $(RM)
  RMDIR = $(RM) -r
endif

ifdef CC65_HOME
  AS = $(CC65_HOME)/bin/ca65
  CC = $(CC65_HOME)/bin/cc65
  CL = $(CC65_HOME)/bin/cl65
  LD = $(CC65_HOME)/bin/ld65
  GRC = $(CC65_HOME)/bin/grc65
else
  AS := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../../bin/cc65*),../../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../../bin/cl65*),../../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)
  GRC := $(if $(wildcard ../../../bin/grc65*),../../../bin/grc65,grc65)
endif

samples: test.s vlir.cvt

test.s: test.grc
	$(GRC) -s test.s test.grc

vlir.cvt: vlir.grc vlir0.s vlir1.s vlir2.s
# using seperate calls here for demonstration purposes:
	$(GRC) -t geos-cbm -s vlir.s vlir.grc
	$(AS) -t geos-cbm vlir.s
	$(AS) -t geos-cbm vlir0.s
	$(AS) -t geos-cbm vlir1.s
	$(AS) -t geos-cbm vlir2.s
	$(LD) -t geos-cbm -o vlir.cvt vlir.o vlir0.o vlir1.o vlir2.o geos-cbm.lib

# you can also do the above in one command:
#	$(CL) -t geos-cbm -o vlir.cvt vlir.grc vlir0.s vlir1.s vlir2.s
	
clean:
	@$(DEL) test.s test.h 2>$(NULLDEV)
	@$(DEL) vlir.s vlir.cvt vlir.c vlir.h 2>$(NULLDEV)
	@$(DEL) *.o 2>$(NULLDEV)
