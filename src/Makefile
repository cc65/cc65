PROGS = ar65  \
        ca65  \
        cc65  \
        cl65  \
        co65  \
        da65  \
        grc65 \
        ld65  \
        od65  \
        sp65

CA65_INC := $(abspath ../asminc)
CC65_INC := $(abspath ../include)
LD65_LIB := $(abspath ../lib)
LD65_OBJ := $(abspath ../lib)
LD65_CFG := $(abspath ../cfg)

CFLAGS += -MMD -MP -O -std=c89 -I common \
          -Wall -Wextra -Wno-char-subscripts -Werror \
          -DCA65_INC=$(CA65_INC) -DCC65_INC=$(CC65_INC) \
          -DLD65_LIB=$(LD65_LIB) -DLD65_OBJ=$(LD65_OBJ) -DLD65_CFG=$(LD65_CFG)

LDLIBS += -lm

all: $(PROGS)

mostlyclean:
	$(RM) -r ../wrk

clean: mostlyclean
	$(RM) -r ../bin

install: all
	$(foreach prog,$(PROGS),$(INSTALL_recipe))

uninstall:
	$(foreach prog,$(PROGS),$(UNINSTALL_recipe))

.PHONY: all $(PROGS) mostlyclean clean install uninstall

.SUFFIXES:

##########

define INSTALL_recipe

ln -s $(abspath ../bin/$(prog)) /usr/local/bin/$(prog)

endef

##########

define UNINSTALL_recipe

$(RM) /usr/local/bin/$(prog)

endef

##########

define OBJS_template

$(1)_OBJS := $$(patsubst %.c,../wrk/%.o,$$(wildcard $(1)/*.c))

$$($(1)_OBJS): | ../wrk/$(1)

../wrk/$(1):
	mkdir -p $$@

DEPS += $$($(1)_OBJS:.o=.d)

endef

##########

define PROG_template

$$(eval $$(call OBJS_template,$(1)))

../bin/$(1): $$($(1)_OBJS) ../wrk/common/common.a | ../bin
	$$(CC) $$(LDFLAGS) -o $$@ $$^ $$(LDLIBS)

$(1): ../bin/$(1)

endef

##########

../wrk/%.o: %.c
	@echo $(CC) $<
	@$(CC) -c $(CFLAGS) -o $@ $<

../bin:
	mkdir $@

$(eval $(call OBJS_template,common))
../wrk/common/common.a: $(common_OBJS)
	$(AR) r $@ $?

$(foreach prog,$(PROGS),$(eval $(call PROG_template,$(prog))))

-include $(DEPS)
