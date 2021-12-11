ifneq ($(shell echo),)
  CMD_EXE = 1
endif

.PHONY: all mostlyclean clean install zip doc html info

.SUFFIXES:

htmldir = $(PREFIX)/share/doc/cc65$(DESTPACKAGE_SUFFIX)/html
infodir = $(PREFIX)/share/info

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

../html ../info:
	@mkdir $@

../html/%.html: %.sgml header.html | ../html
	@cd ../html && linuxdoc -B html -s 0 -T $(TOC_LEVEL) -H ../doc/header.html ../doc/$<

../html/doc.%: doc.% | ../html
	cp $< ../html

../info/%.info: %.sgml | ../info
	@cd ../info && linuxdoc -B info ../doc/$<

clean:
	$(RM) -r ../html ../info

install:
	$(if $(PREFIX),,$(error variable "PREFIX" must be set))
ifeq ($(wildcard ../html),../html)
	$(INSTALL) -d $(DESTDIR)$(htmldir)
	$(INSTALL) -m0644 ../html/*.* $(DESTDIR)$(htmldir)
endif
ifeq ($(wildcard ../info),../info)
	$(INSTALL) -d $(DESTDIR)$(infodir)
	$(INSTALL) -m0644 ../info/*.* $(DESTDIR)$(infodir)
endif

zip:
ifneq "$(wildcard ../html)" ""
	@cd .. && zip cc65 html/*.*
endif

endif # CMD_EXE

all mostlyclean:
