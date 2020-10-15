
# Run 'make SYS=<target>'; or, set a SYS env.
# var. to build for another target system.
SYS ?= geos-cbm

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

DIRLIST = grc

define SUBDIR_recipe

@$(MAKE) -C $(dir) --no-print-directory $@

endef # SUBDIR_recipe

# omitted: dialog.c grphstr.c inittab.c menu.c

# TODO: geosconio.cvt rmvprot.cvt
samples: bitmap-demo.cvt filesel.cvt geosver.cvt getid.cvt hello1.cvt hello2.cvt \
	overlay-demo.cvt vector-demo.cvt yesno.cvt
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))

bitmap.c: logo.pcx
	$(SP) -r logo.pcx -c geos-bitmap -w bitmap.c,ident=bitmap

bitmap-demo.cvt: bitmap.c bitmap-demores.grc bitmap-demo.c
	$(CL) -t $(SYS) -O -o bitmap-demo.cvt -m bitmap-demo.map bitmap-demores.grc bitmap-demo.c
	
filesel.cvt: fileselres.grc filesel.c
	$(CL) -t $(SYS) -O -o filesel.cvt -m filesel.map fileselres.grc filesel.c

geosconio.cvt: geosconiores.grc geosconio.c
	$(CL) -t $(SYS) -O -o geosconio.cvt -m geosconio.map geosconiores.grc geosconio.c

geosver.cvt: geosverres.grc geosver.c
	$(CL) -t $(SYS) -O -o geover.cvt -m geosver.map geosverres.grc geosver.c

getid.cvt: getidres.grc getid.c
	$(CL) -t $(SYS) -O -o getid.cvt -m getid.map getidres.grc getid.c

hello1.cvt: hello1res.grc hello1.c
	$(CL) -t $(SYS) -O -o hello1.cvt -m hello1.map hello1res.grc hello1.c

hello2.cvt: hello2res.grc hello2.c
	$(CL) -t $(SYS) -O -o hello2.cvt -m hello2.map hello2res.grc hello2.c

overlay-demo.cvt: overlay-demores.grc overlay-demo.c
	$(CL) -t $(SYS) -O -o overlay-demo.cvt -m overlay-demo.map overlay-demores.grc overlay-demo.c

rmvprot.cvt: rmvprotres.grc rmvprot.c
	$(CL) -t $(SYS) -O -o rmvprot.cvt -m rmvprot.map rmvprotres.grc rmvprot.c

vector-demo.cvt: vector-demores.grc vector-demo.c
	$(CL) -t $(SYS) -O -o vector-demo.cvt -m vector-demo.map vector-demores.grc vector-demo.c

yesno.cvt: yesnores.grc yesno.c
	$(CL) -t $(SYS) -O -o yesno.cvt -m yesno.map yesnores.grc yesno.c

	
clean:
	@$(DEL) overlay-demores.h 2>$(NULLDEV)
	@$(DEL) bitmap.c 2>$(NULLDEV)
	@$(DEL) *.cvt 2>$(NULLDEV)
	@$(DEL) *.map 2>$(NULLDEV)
	$(foreach dir,$(DIRLIST),$(SUBDIR_recipe))
