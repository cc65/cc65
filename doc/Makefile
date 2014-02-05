.PHONY: all html gh-pages

.SUFFIXES:

all: html

html: $(addprefix ../html/,$(subst .sgml,.html,$(wildcard *.sgml)))

../html:
	mkdir $@

../html/%.html: %.sgml | ../html
	linuxdoc -B html --split=0 --toc=2 $<
	mv *.html $(@D)

../html/coding.html: coding.sgml | ../html
	linuxdoc -B html --split=0 $<
	mv *.html $(@D)

../html/index.html: index.sgml | ../html
	linuxdoc -B html --split=0 $<
	mv *.html $(@D)

GH_PAGES = ../../gh-pages

gh-pages: html
	git clone --branch=gh-pages https://$(GH_TOKEN)@github.com/oliverschmidt/cc65.git $(GH_PAGES)
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.name "Oliver Schmidt"
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.email "ol.sc@web.de"
	rm -rf $(GH_PAGES)/test
	mkdir $(GH_PAGES)/test
	cp -R ../html/* $(GH_PAGES)/test
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git add -A .
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git commit --allow-empty -m "Updated doc from commit $(TRAVIS_COMMIT)."
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git push
