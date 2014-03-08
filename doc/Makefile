ifneq ($(shell echo),)
  CMD_EXE = 1
endif

.PHONY: all mostlyclean clean install zip doc html info gh-pages

.SUFFIXES:

all mostlyclean install:

ifdef CMD_EXE

clean zip doc:

else # CMD_EXE

SGMLS := $(wildcard *.sgml)

../html/coding.html ../html/index.html: \
TOC_LEVEL = 0

TOC_LEVEL = 2

GH_PAGES = ../../gh-pages

clean:
	$(RM) -r ../html ../info

zip:
	cd .. && zip cc65 html/*.*

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

gh-pages: html
ifdef GH_TOKEN
	git clone --branch=gh-pages https://$(GH_TOKEN)@github.com/oliverschmidt/cc65.git $(GH_PAGES)
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.name "Oliver Schmidt"
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.email "ol.sc@web.de"
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config push.default simple
	$(RM) -r $(GH_PAGES)/doc
	mkdir $(GH_PAGES)/doc
	cp ../html/* $(GH_PAGES)/doc
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git add -A doc
	-git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git commit -m "Updated doc from commit $(TRAVIS_COMMIT)."
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git push
endif

endif # CMD_EXE
