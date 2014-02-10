.SUFFIXES:

.PHONY: all bin $(PROGS) mostlyclean clean install avail unavail

ifeq ($(shell echo),)

PROGS = ar65  \
        ca65  \
        cc65  \
        cl65  \
        co65  \
        da65  \
        grc65 \
        ld65  \
        od65  \
        sim65 \
        sp65

bindir  := $(prefix)/bin
datadir := $(if $(prefix),$(prefix)/share/cc65,$(abspath ..))

CA65_INC = $(datadir)/asminc
CC65_INC = $(datadir)/include
LD65_LIB = $(datadir)/lib
LD65_OBJ = $(datadir)/lib
LD65_CFG = $(datadir)/cfg

CFLAGS += -MMD -MP -O -std=c89 -I common \
          -Wall -Wextra -Wno-char-subscripts $(USER_CFLAGS) \
          -DCA65_INC=$(CA65_INC) -DCC65_INC=$(CC65_INC) \
          -DLD65_LIB=$(LD65_LIB) -DLD65_OBJ=$(LD65_OBJ) -DLD65_CFG=$(LD65_CFG)

CFLAGS += $(if $(TRAVIS),-Werror)

LDLIBS += -lm

INSTALL = install

all bin: $(PROGS)

mostlyclean:
	$(RM) -r ../wrk

clean:
	$(RM) -r ../wrk ../bin

install:
	$(if $(prefix),,$(error variable `prefix' must be set))
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) ../bin/* $(DESTDIR)$(bindir)

avail:
	$(foreach prog,$(PROGS),$(AVAIL_recipe))

unavail:
	$(foreach prog,$(PROGS),$(UNAVAIL_recipe))

##########

define AVAIL_recipe

ln -s $(abspath ../bin/$(prog)) /usr/local/bin/$(prog)

endef

##########

define UNAVAIL_recipe

$(RM) /usr/local/bin/$(prog)

endef

##########

define OBJS_template

$1_OBJS := $$(patsubst %.c,../wrk/%.o,$$(wildcard $1/*.c))

$$($1_OBJS): | ../wrk/$1

../wrk/$1:
	@mkdir -p $$@

DEPS += $$($1_OBJS:.o=.d)

endef

##########

define PROG_template

$$(eval $$(call OBJS_template,$1))

../bin/$1$(PROGEXT): $$($1_OBJS) ../wrk/common/common.a | ../bin
	$$(CC) $$(LDFLAGS) -o $$@ $$^ $$(LDLIBS)

$1: ../bin/$1$(PROGEXT)

endef

##########

../wrk/%.o: %.c
	@echo $<
	@$(CC) -c $(CFLAGS) -o $@ $<

../bin:
	@mkdir $@

$(eval $(call OBJS_template,common))
../wrk/common/common.a: $(common_OBJS)
	$(AR) r $@ $?

$(foreach prog,$(PROGS),$(eval $(call PROG_template,$(prog))))

-include $(DEPS)

else # cmd.exe

all bin:
	msbuild cc65.sln /p:configuration=release /consoleloggerparameters:disableconsolecolor

mostlyclean:
	$(if $(wildcard ../wrk),rmdir /s /q ..\wrk)

clean:
	msbuild cc65.sln /p:configuration=release /consoleloggerparameters:disableconsolecolor /target:$@

install avail unavail:

endif # cmd.exe
