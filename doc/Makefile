.PHONY: all doc html gh-pages mostlyclean clean install

.SUFFIXES:

ifeq ($(shell echo),)

SGMLS := $(wildcard *.sgml)

../html/coding.html ../html/index.html: \
TOC_LEVEL = 0

TOC_LEVEL = 2

GH_PAGES = ../../gh-pages

all:

doc: html

html: $(addprefix ../html/,$(SGMLS:.sgml=.html))

mostlyclean:

clean:
	$(RM) -r ../html

../html:
	@mkdir $@

../html/%.html: %.sgml | ../html
	@linuxdoc -B html --split=0 --toc=$(TOC_LEVEL) $<
	@mv $(@F) ../html

gh-pages: html
ifdef GH_TOKEN
	git clone --branch=gh-pages https://$(GH_TOKEN)@github.com/oliverschmidt/cc65.git $(GH_PAGES)
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.name "Oliver Schmidt"
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.email "ol.sc@web.de"
	$(RM) -r $(GH_PAGES)/doc
	mkdir $(GH_PAGES)/doc
	cp -R ../html/* $(GH_PAGES)/doc
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git add -A .
	-git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git commit -m "Updated doc from commit $(TRAVIS_COMMIT)."
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git push
endif

else # cmd.exe

all doc mostlyclean clean install:

endif # cmd.exe
