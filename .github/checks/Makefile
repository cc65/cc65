
ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifdef CMD_EXE

.PHONY: checkstyle

checkstyle:
	$(info INFO: style checks require bash.)

else

.PHONY: checkstyle lineendings tabs lastline spaces noexec

checkstyle: lineendings tabs lastline spaces noexec
	$(info INFO: style checks completed.)

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

endif
