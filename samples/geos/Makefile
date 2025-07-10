# ---- Display info during parsing phase ----
SILENT:=$(findstring s,$(word 1, $(MAKEFLAGS)))
ifneq ($(SILENT),s)
    $(info Using Makefile: $(realpath $(firstword $(MAKEFILE_LIST))) $(MAKECMDGOALS))
endif


# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= geos-cbm

# Comes with the VICE emulator, see http://vice-emu.sourceforge.net/
C1541 ?= c1541

# If SYS was given on the commandline, redirect "c64" to "geos-cbm" and
# "apple2enh" to "geos-apple"
ifeq ($(origin SYS),command line)
  ifeq ($(SYS),c64)
    override SYS = geos-cbm
    ifneq ($(SILENT),s)
      $(info GEOS: c64 -> geos-cbm)
    endif
  endif
  ifeq ($(SYS),apple2enh)
    override SYS = geos-apple
    ifneq ($(SILENT),s)
      $(info GEOS: apple2enh -> geos-apple)
    endif
  endif
endif

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
  SP = $(CC65_HOME)/bin/sp65
else
  AS := $(if $(wildcard ../../bin/ca65*),../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../bin/cc65*),../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../bin/cl65*),../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../bin/ld65*),../../bin/ld65,ld65)
  SP := $(if $(wildcard ../../bin/sp65*),../../bin/sp65,sp65)
endif

ifeq ($(SILENT),s)
    QUIET = 1
endif

ifdef QUIET
    .SILENT:
endif

DIRLIST = grc

define SUBDIR_recipe

@+$(MAKE) SYS=$(SYS) -C $(dir) --no-print-directory $@

endef # SUBDIR_recipe

EXELIST_geos-cbm = \
	bitmap-demo.cvt \
	filesel.cvt \
	geosver.cvt \
	getid.cvt \
	hello1.cvt \
	hello2.cvt \
	overlay-demo.cvt \
	vector-demo.cvt \
	yesno.cvt

EXELIST_geos-apple = \
	bitmap-demo.cvt \
	filesel.cvt \
	hello1.cvt \
	hello2.cvt \
	overlay-demo.cvt \
	vector-demo.cvt \
	yesno.cvt

# omitted: dialog.c grphstr.c inittab.c menu.c
# TODO: geosconio.cvt rmvprot.cvt

ifneq ($(EXELIST_$(SYS)),)
samples: $(EXELIST_$(SYS))
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))

define samples-geos
$(C1541) -attach $(0).d64 -geoswrite $(1);
endef

samples-geos: $(EXELIST_$(SYS))
	$(C1541) -format "$@,01" d64 $@.d64
	$(foreach tool,$(EXELIST_$(SYS)),$(call samples-geos,$(tool)))
else
samples:
  ifeq ($(MAKELEVEL),0)
	@echo "info: geos samples not available for" $(SYS)
  else
# suppress the "nothing to be done for 'samples' message
	@echo > $(NULLDEV)
  endif
endif

bitmap.c: logo.pcx
	$(SP) -r logo.pcx -c geos-bitmap -w bitmap.c,ident=bitmap

bitmap-demo.cvt: bitmap.c bitmap-demores.grc bitmap-demo.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m bitmap-demo.map bitmap-demores.grc bitmap-demo.c

filesel.cvt: fileselres.grc filesel.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m filesel.map fileselres.grc filesel.c

geosconio.cvt: geosconiores.grc geosconio.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m geosconio.map geosconiores.grc geosconio.c

geosver.cvt: geosverres.grc geosver.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m geosver.map geosverres.grc geosver.c

getid.cvt: getidres.grc getid.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m getid.map getidres.grc getid.c

hello1.cvt: hello1res.grc hello1.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m hello1.map hello1res.grc hello1.c

hello2.cvt: hello2res.grc hello2.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m hello2.map hello2res.grc hello2.c

overlay-demo.cvt: overlay-demores.grc overlay-demo.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m overlay-demo.map overlay-demores.grc overlay-demo.c

rmvprot.cvt: rmvprotres.grc rmvprot.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m rmvprot.map rmvprotres.grc rmvprot.c

vector-demo.cvt: vector-demores.grc vector-demo.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m vector-demo.map vector-demores.grc vector-demo.c

yesno.cvt: yesnores.grc yesno.c
	$(if $(QUIET),echo $(SYS):$@)
	$(CL) -t $(SYS) -O -o $@ -m yesno.map yesnores.grc yesno.c


clean:
	@$(DEL) overlay-demores.h 2>$(NULLDEV)
	@$(DEL) bitmap.c 2>$(NULLDEV)
	@$(DEL) *.cvt 2>$(NULLDEV)
	@$(DEL) *.map 2>$(NULLDEV)
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))
