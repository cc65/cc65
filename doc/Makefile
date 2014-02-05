.PHONY: all doc html gh-pages mostlyclean clean install

.SUFFIXES:

ifeq ($(shell echo),)

SGMLS := $(wildcard *.sgml)

GH_PAGES = ../../gh-pages

all:

doc: html

mostlyclean:

clean:
	$(RM) -r ../html

html: $(addprefix ../html/,$(SGMLS:.sgml=.html))

../html:
	mkdir $@

../html/%.html: %.sgml | ../html
	linuxdoc -B html --split=0 --toc=2 $<
	mv $(@F) ../html

gh-pages: html
ifdef GH_TOKEN
	git clone --branch=gh-pages https://$(GH_TOKEN)@github.com/oliverschmidt/cc65.git $(GH_PAGES)
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.name "Oliver Schmidt"
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.email "ol.sc@web.de"
	$(RM) -r $(GH_PAGES)/test
	mkdir $(GH_PAGES)/test
	cp -R ../html/* $(GH_PAGES)/test
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git add -A .
	-git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git commit -m "Updated doc from commit $(TRAVIS_COMMIT)."
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git push
endif

else # cmd.exe

all doc mostlyclean clean install:

endif # cmd.exe
