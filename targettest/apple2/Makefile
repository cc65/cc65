# For this one see https://applecommander.github.io/
AC ?= ac.jar

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
else
  AS := $(if $(wildcard ../../../bin/ca65*),../../../bin/ca65,ca65)
  CC := $(if $(wildcard ../../../bin/cc65*),../../../bin/cc65,cc65)
  CL := $(if $(wildcard ../../../bin/cl65*),../../../bin/cl65,cl65)
  LD := $(if $(wildcard ../../../bin/ld65*),../../../bin/ld65,ld65)
endif

all: hgrshow hgrtest dhgrshow

disk: hgr.dsk dhgr.dsk

hgr.dsk: hgrshow hgrtest
	cp prodos.dsk $@
	java -jar $(AC) -as $@ hgrshow <hgrshow
	java -jar $(AC) -as $@ hgrtest <hgrtest
	java -jar $(AC) -p  $@ astronaut.hgr  bin 0x2000 <astronaut.hgr
	java -jar $(AC) -p  $@ chips.hgr      bin 0x2000 <chips.hgr
	java -jar $(AC) -p  $@ macrometer.hgr bin 0x2000 <macrometer.hgr
	java -jar $(AC) -p  $@ mariner.hgr    bin 0x2000 <mariner.hgr
	java -jar $(AC) -p  $@ rose.hgr       bin 0x2000 <rose.hgr
	java -jar $(AC) -p  $@ werner.hgr     bin 0x2000 <werner.hgr
	java -jar $(AC) -p  $@ winston.hgr    bin 0x2000 <winston.hgr

hgrshow: hgrshow.c
	$(CL) -Oirs -t apple2 --start-addr 0x4000 -m hgrshow.map $^

hgrtest: hgrtest.c werner.s
	$(CL) -Oirs -t apple2 -C apple2-hgr.cfg -m hgrtest.map $^

dhgr.dsk: dhgrshow
	cp prodos.dsk $@
	java -jar $(AC) -as $@ dhgrshow <dhgrshow
	java -jar $(AC) -p  $@ catface.dhgr  bin 0x2000 <catface.dhgr
	java -jar $(AC) -p  $@ gatsby.dhgr   bin 0x2000 <gatsby.dhgr
	java -jar $(AC) -p  $@ girl.dhgr     bin 0x2000 <girl.dhgr
	java -jar $(AC) -p  $@ monarch.dhgr  bin 0x2000 <monarch.dhgr
	java -jar $(AC) -p  $@ superman.dhgr bin 0x2000 <superman.dhgr
	java -jar $(AC) -p  $@ venice.dhgr   bin 0x2000 <venice.dhgr

dhgrshow: dhgrshow.c
	$(CL) -Oirs -t apple2enh --start-addr 0x4000 -m dhgrshow.map $^

clean:
	@$(DEL) hgr.dsk dhgr.dsk 2>$(NULLDEV)
	@$(DEL) hgrshow hgrshow.map 2>$(NULLDEV)
	@$(DEL) hgrtest hgrtest.map 2>$(NULLDEV)
	@$(DEL) dhgrshow dhgrshow.map 2>$(NULLDEV)
