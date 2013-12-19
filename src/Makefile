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

ifdef prefix
  CA65_INC = $(prefix)/lib/cc65/asminc
  CC65_INC = $(prefix)/lib/cc65/include
  LD65_LIB = $(prefix)/lib/cc65/lib
  LD65_OBJ = $(prefix)/lib/cc65/lib
  LD65_CFG = $(prefix)/lib/cc65/cfg
else
  CA65_INC := $(abspath ../asminc)
  CC65_INC := $(abspath ../include)
  LD65_LIB := $(abspath ../lib)
  LD65_OBJ := $(abspath ../lib)
  LD65_CFG := $(abspath ../cfg)
endif

CFLAGS += -MMD -MP -O -std=c89 -I common \
          -Wall -Wextra -Wno-char-subscripts -Werror $(USER_CFLAGS) \
          -DCA65_INC=$(CA65_INC) -DCC65_INC=$(CC65_INC) \
          -DLD65_LIB=$(LD65_LIB) -DLD65_OBJ=$(LD65_OBJ) -DLD65_CFG=$(LD65_CFG)

LDLIBS += -lm

.SUFFIXES:

.PHONY: all bin $(PROGS) mostlyclean clean avail unavail

all bin: $(PROGS)

mostlyclean:
	$(RM) -r ../wrk

clean:
	$(RM) -r ../wrk ../bin

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

.SUFFIXES:

.PHONY: all bin mostlyclean clean

all bin:
	msbuild cc65.sln /p:configuration=release /consoleloggerparameters:disableconsolecolor

mostlyclean:
	$(if $(wildcard ../wrk),rmdir /s /q ..\wrk)

clean:
	msbuild cc65.sln /p:configuration=release /consoleloggerparameters:disableconsolecolor /target:$@

endif # cmd.exe
