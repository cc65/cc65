GH_PAGES = ../../gh-pages

gh-pages:
	git clone --branch=gh-pages https://$(GH_TOKEN)@github.com/oliverschmidt/cc65.git $(GH_PAGES)
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.name "Oliver Schmidt"
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git config user.email "ol.sc@web.de"
	echo date > $(GH_PAGES)/testfile.txt
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git add -A
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git commit -m "Updated doc from commit $(TRAVIS_COMMIT)."
	git --work-tree=$(GH_PAGES) --git-dir=$(GH_PAGES)/.git push
