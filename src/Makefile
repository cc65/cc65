ifneq ($(shell echo),)
  CMD_EXE = 1
endif

ifneq ($(V),1)
  Q=@
else
  Q=
endif

PROGS = ar65     \
        ca65     \
        cc65     \
        chrcvt65 \
        cl65     \
        co65     \
        da65     \
        grc65    \
        ld65     \
        od65     \
        sim65    \
        sp65

.PHONY: all mostlyclean clean install zip avail unavail bin $(PROGS)

.SUFFIXES:

bindir  := $(PREFIX)/bin
datadir := $(PREFIX)/share/cc65

CA65_INC = $(datadir)/asminc
CC65_INC = $(datadir)/include
CL65_TGT = $(datadir)/target
LD65_LIB = $(datadir)/lib
LD65_OBJ = $(datadir)/lib
LD65_CFG = $(datadir)/cfg

ifdef CMD_EXE
  NULLDEV = nul:
  DIRLIST = $(strip $(foreach dir,$1,$(wildcard $(dir))))
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = $(if $(DIRLIST),rmdir /s /q $(subst /,\,$(DIRLIST)))
else
  NULLDEV = /dev/null
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
endif

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

ifdef CROSS_COMPILE
  $(info CC: $(CC))
  $(info AR: $(AR))
endif

ifdef USER_CFLAGS
  $(info USER_CFLAGS: $(USER_CFLAGS))
endif

ifndef BUILD_ID
  BUILD_ID := Git $(shell git rev-parse --short HEAD 2>$(NULLDEV) || svnversion 2>$(NULLDEV))
  ifneq ($(words $(BUILD_ID)),2)
    BUILD_ID := N/A
  endif
endif
$(info BUILD_ID: $(BUILD_ID))

CFLAGS += -MMD -MP -O3 -I common \
          -Wall -Wextra -Wno-char-subscripts $(USER_CFLAGS) \
          -DCA65_INC="\"$(CA65_INC)\"" -DCC65_INC="\"$(CC65_INC)\"" -DCL65_TGT="\"$(CL65_TGT)\"" \
          -DLD65_LIB="\"$(LD65_LIB)\"" -DLD65_OBJ="\"$(LD65_OBJ)\"" -DLD65_CFG="\"$(LD65_CFG)\"" \
          -DBUILD_ID="\"$(BUILD_ID)\""

LDLIBS += -lm

ifdef CMD_EXE
  EXE_SUFFIX=.exe
endif

ifdef CROSS_COMPILE
  EXE_SUFFIX=.exe
endif

all bin: $(PROGS)

mostlyclean:
	$(call RMDIR,../wrk)

clean:
	$(call RMDIR,../wrk ../bin)

ifdef CMD_EXE

install avail unavail:

else # CMD_EXE

INSTALL = install

define AVAIL_recipe

ln -s $(abspath ../bin/$(prog)) /usr/local/bin/$(prog)

endef # AVAIL_recipe

define UNAVAIL_recipe

$(RM) /usr/local/bin/$(prog)

endef # UNAVAIL_recipe

install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) ../bin/* $(DESTDIR)$(bindir)

avail:
ifneq ($(patsubst %,../bin/%,$(PROGS)),$(wildcard $(patsubst %,../bin/%,$(PROGS))))
	$(error executables are missing, please run make first)
else
	$(foreach prog,$(PROGS),$(AVAIL_recipe))
endif

unavail:
	$(foreach prog,$(PROGS),$(UNAVAIL_recipe))

endif # CMD_EXE

zip:
	@cd .. && zip cc65 bin/*
	@echo 'https://github.com/cc65/cc65/commits/'$(word 2,$(BUILD_ID))|zip -zq ../cc65

define OBJS_template

$1_OBJS := $$(patsubst %.c,../wrk/%.o,$$(wildcard $1/*.c))

$$($1_OBJS): | ../wrk/$1

../wrk/$1:
	@$$(call MKDIR,$$@)

DEPS += $$($1_OBJS:.o=.d)

endef # OBJS_template

define PROG_template

$$(eval $$(call OBJS_template,$1))

../bin/$1$(EXE_SUFFIX): $$($1_OBJS) ../wrk/common/common.a | ../bin
	$$(CC) $$(LDFLAGS) -o $$@ $$^ $$(LDLIBS)

$1: ../bin/$1$(EXE_SUFFIX)

endef # PROG_template

../wrk/%.o: %.c
	@echo $<
	$(Q)$(CC) -c $(CFLAGS) -o $@ $<

../bin:
	@$(call MKDIR,$@)

$(eval $(call OBJS_template,common))

../wrk/common/common.a: $(common_OBJS)
	$(AR) r $@ $?

$(foreach prog,$(PROGS),$(eval $(call PROG_template,$(prog))))

-include $(DEPS)
