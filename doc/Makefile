gh-pages:
	git clone --branch=gh-pages https://${GH_TOKEN}@github.com/oliverschmidt/cc65.git ../gh-pages
	cd ../gh-pages
	git config user.name "Oliver Schmidt"
	git config user.email "ol.sc@web.de"
	echo date > testfile.txt
	git add -A
	git commit -m "Updated doc from commit $(TRAVIS_COMMIT)."
	git push
