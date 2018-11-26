ifneq ($(shell echo),)
  CMD_EXE = 1
endif

.PHONY: all mostlyclean clean install zip doc html info pdf

.SUFFIXES:

htmldir = $(PREFIX)/share/doc/cc65$(DESTPACKAGE_SUFFIX)/html
infodir = $(PREFIX)/share/info
pdfdir = $(PREFIX)/share/doc/cc65$(DESTPACKAGE_SUFFIX)/pdf

ifdef CMD_EXE

# building of documentation is not supported on plain Windows
doc clean install zip:

else # CMD_EXE

SGMLS := $(wildcard *.sgml)

TOC_LEVEL = 2

../html/coding.html ../html/index.html: TOC_LEVEL = 0

RMDIR = $(RM) -rf
INSTALL = install
LATEX = latex

doc: html info

html: $(addprefix ../html/,$(SGMLS:.sgml=.html) doc.css doc.png)

info: $(addprefix ../info/,$(SGMLS:.sgml=.info))

docwrk: $(addprefix ../docwrk/,$(SGMLS:.sgml=.dvi))

pdf: $(addprefix ../pdf/,$(SGMLS:.sgml=.pdf))

../html ../info ../pdf ../docwrk:
	@mkdir $@

../html/%.html: %.sgml header.html | ../html
	@cd ../html && linuxdoc -B html -s 0 -T $(TOC_LEVEL) -H ../doc/header.html ../doc/$<

../html/doc.%: doc.% | ../html
	cp $< ../html

../info/%.info: %.sgml | ../info
	@cd ../info && linuxdoc -B info ../doc/$<

.PRECIOUS: ../docwrk/%.tex
../docwrk/%.tex: %.sgml | ../docwrk
	@cd ../docwrk && linuxdoc -B latex ../doc/$<

../docwrk/%.dvi: ../docwrk/%.tex
	@cd ../docwrk && \
	rm -f $*.make.log; \
	(logHash=aaa; \
	lastLogHash=bbb; \
	count=1; \
	echo -n "Generating $*.pdf : "; \
	while [ "$$lastLogHash" != "$$logHash" ]; \
	  do \
	   echo -n "..$$count"; \
	   count=$$(expr $$count + 1);\
	   $(LATEX) $< >> $*.make.log; \
	   lastLogHash=$$logHash; \
	   logHash=$$(md5sum $*.log); \
	done;)
	@echo "..done."

../pdf/%.pdf: ../docwrk/%.dvi | ../pdf
	@cd ../pdf && dvipdf $<

clean:
	$(RMDIR) ../html ../info ../pdf ../docwrk

install:
	$(if $(PREFIX),,$(error variable `PREFIX' must be set))
ifeq ($(wildcard ../html),../html)
	$(INSTALL) -d $(DESTDIR)$(htmldir)
	$(INSTALL) -m0644 ../html/*.* $(DESTDIR)$(htmldir)
endif
ifeq ($(wildcard ../info),../info)
	$(INSTALL) -d $(DESTDIR)$(infodir)
	$(INSTALL) -m0644 ../info/*.* $(DESTDIR)$(infodir)
endif
ifeq ($(wildcard ../pdf),../pdf)
	$(INSTALL) -d $(DESTDIR)$(pdfdir)
	$(INSTALL) -m0644 ../pdf/*.* $(DESTDIR)$(pdfdir)
endif

zip:
ifneq "$(wildcard ../html)" ""
	@cd .. && zip cc65 html/*.*
endif

endif # CMD_EXE

all mostlyclean:
