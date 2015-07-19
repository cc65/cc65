ifneq ($(shell echo),)
  CMD_EXE = 1
endif

.PHONY: all mostlyclean clean install zip doc html info

.SUFFIXES:

htmldir = $(prefix)/share/doc/cc65$(DESTPACKAGE_SUFFIX)/html
infodir = $(prefix)/share/info

all mostlyclean:

ifdef CMD_EXE

clean install zip doc:

else # CMD_EXE

SGMLS := $(wildcard *.sgml)

../html/coding.html ../html/index.html: \
TOC_LEVEL = 0

TOC_LEVEL = 2

INSTALL = install

clean:
	$(RM) -r ../html ../info

install:
	$(if $(prefix),,$(error variable `prefix' must be set))
ifeq ($(wildcard ../html),../html)
	$(INSTALL) -d $(DESTDIR)$(htmldir)
	$(INSTALL) -m644 ../html/*.* $(DESTDIR)$(htmldir)
endif
ifeq ($(wildcard ../info),../info)
	$(INSTALL) -d $(DESTDIR)$(infodir)
	$(INSTALL) -m644 ../info/*.* $(DESTDIR)$(infodir)
endif

zip:
	@cd .. && zip cc65 html/*.*

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

endif # CMD_EXE
