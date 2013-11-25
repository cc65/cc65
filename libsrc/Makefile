CBMS = c128   \
       c16    \
       c64    \
       cbm510 \
       cbm610 \
       pet    \
       plus4  \
       vic20

GEOS = geos-apple \
       geos-cbm

TARGETS = apple2    \
          apple2enh \
          atari     \
          atarixl   \
          atmos     \
          $(CBMS)   \
          $(GEOS)   \
          lynx      \
          nes       \
          sim6502   \
          sim65c02  \
          supervision

DRVTYPES = emd \
           joy \
           mou \
           ser \
           tgi

# Every target requires its individual vpath setting but the vpath directive
# acts globally. Therefore each target is built in a separate make instance.

ifeq ($(words $(MAKECMDGOALS)),1)
  ifeq ($(MAKECMDGOALS),$(filter $(MAKECMDGOALS),$(TARGETS)))
    TARGET = $(MAKECMDGOALS)
  endif
endif

DIRLIST = $(strip $(foreach dir,$1,$(wildcard $(dir))))

ifeq ($(shell echo),)
  MKDIR = mkdir -p $1
  RMDIR = $(RM) -r $1
else
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = $(if $(DIRLIST),rmdir /s /q $(subst /,\,$(DIRLIST)))
endif

.SUFFIXES:

.PHONY: all lib $(TARGETS) mostlyclean clean

ifndef TARGET

all lib: $(TARGETS)

$(TARGETS):
	@$(MAKE) --no-print-directory $@

mostlyclean:
	$(call RMDIR,../wrk)

clean:
	$(call RMDIR,../wrk ../lib ../targetutil $(addprefix ../,$(DRVTYPES)))

else # TARGET

CA65FLAGS =
CC65FLAGS = -Osir -W error

EXTZP = cbm510 \
        cbm610 \
        lynx

MKINC = $(GEOS) \
        atari   \
        nes

TARGETUTIL = apple2 \
             geos-apple

GEOSDIRS = common      \
           conio       \
           disk        \
           dlgbox      \
           file        \
           graph       \
           memory      \
           menuicon    \
           mousesprite \
           process     \
           runtime     \
           system

ifeq ($(TARGET),apple2enh)
  SRCDIR = apple2
  OBJPFX = a2
  DRVPFX = a2e
else ifeq ($(TARGET),atarixl)
  SRCDIR = atari
  OBJPFX = atr
  DRVPFX = atrx
else ifeq ($(TARGET),sim65c02)
  SRCDIR = sim6502
else
  SRCDIR = $(TARGET)
endif

SRCDIRS = $(SRCDIR)

ifeq ($(TARGET),$(filter $(TARGET),$(CBMS)))
  SRCDIRS += cbm
endif

ifeq ($(TARGET),$(filter $(TARGET),$(GEOS)))
  SRCDIRS += $(addprefix $(TARGET)/,  $(GEOSDIRS))
  SRCDIRS += $(addprefix geos-common/,$(GEOSDIRS))
endif

SRCDIRS += common   \
           conio    \
           dbg      \
           em       \
           joystick \
           mouse    \
           runtime  \
           serial   \
           tgi      \
           zlib

vpath %.s $(SRCDIRS)
vpath %.c $(SRCDIRS)

OBJS := $(patsubst %.s,%.o,$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.s)))
OBJS += $(patsubst %.c,%.o,$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c)))

OBJS := $(addprefix ../wrk/$(TARGET)/,$(sort $(notdir $(OBJS))))

DEPS = $(OBJS:.o=.d)

EXTRA_SRCPAT = $(SRCDIR)/extra/%.s
EXTRA_OBJPAT = ../lib/$(TARGET)-%.o
EXTRA_OBJS := $(patsubst $(EXTRA_SRCPAT),$(EXTRA_OBJPAT),$(wildcard $(SRCDIR)/extra/*.s))

ZPOBJ = ../wrk/$(TARGET)/zeropage.o
ifeq ($(TARGET),$(filter $(TARGET),$(EXTZP)))
  ZPOBJ += ../wrk/$(TARGET)/extzp.o
endif

ifeq ($(SRCDIR),$(filter $(SRCDIR),$(MKINC)))
  include $(SRCDIR)/Makefile.inc
endif

ifeq ($(SRCDIR),$(filter $(SRCDIR),$(TARGETUTIL)))
  include $(SRCDIR)/targetutil/Makefile.inc
endif

##########

define DRVTYPE_template

$1_SRCDIR = $$(SRCDIR)/$1
$1_OBJDIR = ../wrk/$$(TARGET)/$1
$1_DRVDIR = ../$1

$1_OBJPAT = $$($1_OBJDIR)/$$(OBJPFX)%.o
$1_DRVPAT = $$($1_DRVDIR)/$$(DRVPFX)%.$1
$1_STCPAT = ../wrk/$$(TARGET)/$$(DRVPFX)%-$1.o

$1_OBJS := $$(patsubst $$($1_SRCDIR)/%.s,$$($1_OBJDIR)/%.o,$$(wildcard $$($1_SRCDIR)/*.s))

$1_DRVS = $$(patsubst $$($1_OBJPAT),$$($1_DRVPAT),$$($1_OBJS))

$1_STCS = $$(patsubst $$($1_DRVPAT),$$($1_STCPAT),$$($1_DRVS))

$$($1_OBJS): | $$($1_OBJDIR)

$$($1_DRVPAT): $$($1_OBJPAT) $$(ZPOBJ) | $$($1_DRVDIR)
	@echo $$(TARGET) - $$(<F)
	@$$(LD65) -o $$@ -t module $$^

$$($1_OBJDIR) $$($1_DRVDIR):
	@$$(call MKDIR,$$@)

$(TARGET): $$($1_DRVS)

$$($1_STCPAT): $$($1_DRVPAT)
	@echo $$(TARGET) - $$(<F)
	@$$(CO65) -o $$(@:.o=.s) --code-label _$$(subst -,_,$$(subst .,_,$$(<F))) $$<
	@$$(CA65) -t $$(TARGET) -o $$@ $$(@:.o=.s)

OBJS += $$($1_STCS)

DEPS += $$($1_OBJS:.o=.d)

endef

##########

$(foreach drvtype,$(DRVTYPES),$(eval $(call DRVTYPE_template,$(drvtype))))

AR65 := $(if $(wildcard ../bin/ar65*),../bin/ar65,ar65)
CA65 := $(if $(wildcard ../bin/ca65*),../bin/ca65,ca65)
CC65 := $(if $(wildcard ../bin/cc65*),../bin/cc65,cc65)
CO65 := $(if $(wildcard ../bin/co65*),../bin/co65,co65)
LD65 := $(if $(wildcard ../bin/ld65*),../bin/ld65,ld65)

export CC65_HOME := $(abspath ..)

##########

define ASSEMBLE_recipe

$(if $(TRAVIS),,@echo $(TARGET) - $<)
@$(CA65) -t $(TARGET) $(CA65FLAGS) --create-dep $(@:.o=.d) -o $@ $<

endef

##########

define COMPILE_recipe

$(if $(TRAVIS),,@echo $(TARGET) - $<)
@$(CC65) -t $(TARGET) $(CC65FLAGS) --create-dep $(@:.o=.d) --dep-target $@ -o $(@:.o=.s) $<
@$(CA65) -t $(TARGET) -o $@ $(@:.o=.s)

endef

##########

../wrk/$(TARGET)/%.o: %.s | ../wrk/$(TARGET)
	$(ASSEMBLE_recipe)

../wrk/$(TARGET)/%.o: %.c | ../wrk/$(TARGET)
	$(COMPILE_recipe)

$(EXTRA_OBJPAT): $(EXTRA_SRCPAT) | ../lib
	@echo $(TARGET) - $(<F)
	@$(CA65) -t $(TARGET) $(CA65FLAGS) -o $@ $<

../lib/$(TARGET).lib: $(OBJS) | ../lib
	$(AR65) a $@ $?

../wrk/$(TARGET) ../lib ../targetutil:
	@$(call MKDIR,$@)

$(TARGET): $(EXTRA_OBJS) ../lib/$(TARGET).lib

-include $(DEPS)

endif # TARGET
