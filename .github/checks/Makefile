
.PHONY: tabs

check: tabs lastline

tabs: tabs.sh
	@./tabs.sh
	
lastline: lastline.sh
	@./lastline.sh
	
# checks that will currently fail (on a lot of files), so they are not included
# in the general "check" action
spaces: spaces.sh
	@./spaces.sh
