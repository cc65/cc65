
ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE

.PHONY: checkstyle sorted

checkstyle:
	$(info INFO: style checks require bash.)
sorted:
	$(info INFO: table checks require bash.)

else

.PHONY: checkstyle lineendings tabs lastline spaces noexec sorted

all: checkstyle sorted

checkstyle: lineendings tabs lastline spaces noexec

lineendings: lineendings.sh
	@./lineendings.sh

tabs: tabs.sh
	@./tabs.sh

lastline: lastline.sh
	@./lastline.sh

spaces: spaces.sh
	@./spaces.sh

noexec: noexec.sh
	@./noexec.sh

sorted: sorted.sh sorted_codeopt.sh
	@./sorted.sh
	@./sorted_codeopt.sh

endif
