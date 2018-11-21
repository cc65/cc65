ifneq ($(shell echo),)
  CMD_EXE = 1
endif

.PHONY: all mostlyclean clean install zip doc html info

.SUFFIXES:

htmldir = $(PREFIX)/share/doc/cc65$(DESTPACKAGE_SUFFIX)/html
infodir = $(PREFIX)/share/info
pdfdir = $(PREFIX)/share/doc/cc65$(DESTPACKAGE_SUFFIX)/pdf

ifdef CMD_EXE

doc clean install zip:

else # CMD_EXE

SGMLS := $(wildcard *.sgml)

../html/coding.html ../html/index.html: \
TOC_LEVEL = 0

TOC_LEVEL = 2

INSTALL = install

doc: html info

html: $(addprefix ../html/,$(SGMLS:.sgml=.html) doc.css doc.png)

info: $(addprefix ../info/,$(SGMLS:.sgml=.info))

tex: $(addprefix ../tex/,$(SGMLS:.sgml=.dvi))

pdf: $(addprefix ../pdf/,$(SGMLS:.sgml=.pdf))

../html ../info ../pdf ../tex:
	@mkdir $@

../html/%.html: %.sgml header.html | ../html
	@cd ../html && linuxdoc -B html -s 0 -T $(TOC_LEVEL) -H ../doc/header.html ../doc/$<

../html/doc.%: doc.% | ../html
	cp $< ../html

../info/%.info: %.sgml | ../info
	@cd ../info && linuxdoc -B info ../doc/$<

../tex/%.tex: %.sgml | ../tex
	@cd ../tex && linuxdoc -B latex ../doc/$<

.PRECIOUS ../tex/%.dvi: ../tex/%.tex | ../tex
	@cd ../tex && (latex $< && latex $< && latex $< && latex $<)

../pdf/%.pdf: ../tex/%.dvi | ../pdf
	@cd ../pdf && dvipdf $<

clean:
	$(RM) -r ../html ../info ../tex ../pdf

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
