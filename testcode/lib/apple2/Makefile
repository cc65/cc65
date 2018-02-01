# For this one see https://applecommander.github.io/
AC ?= ac.jar

CL      = cl65
CLFLAGS = -t apple2 -C apple2-hgr.cfg -Oirs

hgrtest.dsk: hgrtest
	cp prodos.dsk $@
	java -jar $(AC) -cc65 $@ hgrtest bin <hgrtest

hgrtest: hgrtest.c werner.s
	$(CL) $(CLFLAGS) -m hgrtest.map $^
