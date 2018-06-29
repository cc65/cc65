# Sample makefile using a preprocessor against info files
# and the --sync-lines option

CPP = env LANG=C cpp
CPPFLAGS = # -DTEST_ERROR

ASMS = fixed.s bank0.s bank1.s
DAIS = fixed.dai bank0.dai bank1.dai

.SUFFIXES: .da .dai .s
.PHONY: all clean maintainer-clean
.SECONDARY: $(DAIS)

.da.dai:
	$(CPP) -o $@ $(CPPFLAGS) $<

.dai.s:
	da65 --sync-lines -o $@ -i $< image.bin

all: $(ASMS)

clean:
	rm -f $(ASMS)

maintainer-clean: clean
	rm -f $(DAIS)

$(DAIS): fixed.da
