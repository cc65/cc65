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

TARGETS = $(CBMS)   \
          $(GEOS)   \
          apple2    \
          apple2enh \
          atari     \
          atmos     \
          lynx      \
          nes       \
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
	$(call RMDIR,../wrk ../lib $(addprefix ../,$(DRVTYPES)))

else # TARGET

CFLAGS += -Osir

EXTZP = cbm510 \
        cbm610 \
        lynx

MKINC = $(GEOS) \
        atari   \
        nes

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
  OBJPFX = a2
  DRVPFX = a2e
  DIRS = apple2
else
  DIRS = $(TARGET)
endif

ifeq ($(TARGET),$(filter $(TARGET),$(CBMS)))
  DIRS += cbm
endif

ifeq ($(TARGET),$(filter $(TARGET),$(GEOS)))
  DIRS += $(addprefix $(TARGET)/,  $(GEOSDIRS))
  DIRS += $(addprefix geos-common/,$(GEOSDIRS))
endif

DIRS += common   \
        conio    \
        dbg      \
        em       \
        joystick \
        mouse    \
        runtime  \
        serial   \
        tgi      \
        zlib

vpath %.s $(DIRS)
vpath %.c $(DIRS)

OBJS := $(patsubst %.s,%.o,$(foreach dir,$(DIRS),$(wildcard $(dir)/*.s)))
OBJS += $(patsubst %.c,%.o,$(foreach dir,$(DIRS),$(wildcard $(dir)/*.c)))

OBJS := $(addprefix ../wrk/$(TARGET)/,$(sort $(notdir $(OBJS))))

DEPS = $(OBJS:.o=.d)

EXTRA_SRCPAT = $(firstword $(DIRS))/extra/%.s
EXTRA_OBJPAT = ../lib/$(TARGET)-%.o
EXTRA_OBJS := $(patsubst $(EXTRA_SRCPAT),$(EXTRA_OBJPAT),$(wildcard $(firstword $(DIRS))/extra/*.s))

ZPOBJ = ../wrk/$(TARGET)/zeropage.o
ifeq ($(TARGET),$(filter $(TARGET),$(EXTZP)))
  ZPOBJ += ../wrk/$(TARGET)/extzp.o
endif

ifeq ($(TARGET),$(filter $(TARGET),$(MKINC)))
  include $(TARGET)/Makefile.inc
endif

##########

define DRVTYPE_template

$1_SRCDIR = $$(firstword $$(DIRS))/$1
$1_OBJDIR = ../wrk/$$(TARGET)/$1
$1_DRVDIR = ../$1

$1_OBJPAT = $$($1_OBJDIR)/$$(OBJPFX)%.o
$1_DRVPAT = $$($1_DRVDIR)/$$(DRVPFX)%.$1
$1_STCPAT = ../wrk/$$(TARGET)/$$(DRVPFX)%-$1.o

$1_OBJS := $$(patsubst $$($1_SRCDIR)/%.s,$$($1_OBJDIR)/%.o,$$(wildcard $$($1_SRCDIR)/*.s))

$1_DRVS = $$(patsubst $$($1_OBJPAT),$$($1_DRVPAT),$$($1_OBJS))

$1_STCS = $$(patsubst $$($1_DRVPAT),$$($1_STCPAT),$$($1_DRVS))

$$($1_OBJS): | $$($1_OBJDIR)

$$($1_OBJDIR):
	@$$(call MKDIR,$$@)

$$($1_DRVPAT): $$($1_OBJPAT) $$(ZPOBJ) | $$($1_DRVDIR)
	@echo $$(TARGET) - $$(@F)
	@$$(LD) -o $$@ -t module $$^

$$($1_DRVDIR):
	@$$(call MKDIR,$$@)

$(TARGET): $$($1_DRVS)

$$($1_STCPAT): $$($1_DRVPAT)
	@echo $$(TARGET) - $$(<F)
	@$$(CO) -o $$(@:.o=.s) --code-label _$$(subst -,_,$$(subst .,_,$$(<F))) $$<
	@$$(CA) -t $$(TARGET) -o $$@ $$(@:.o=.s)

OBJS += $$($1_STCS)

DEPS += $$($1_OBJS:.o=.d)

endef

##########

$(foreach drvtype,$(DRVTYPES),$(eval $(call DRVTYPE_template,$(drvtype))))

AR = $(if $(wildcard ../bin/ar65*),../bin/ar65,ar65)
CA = $(if $(wildcard ../bin/ca65*),../bin/ca65,ca65)
CC = $(if $(wildcard ../bin/cc65*),../bin/cc65,cc65)
CO = $(if $(wildcard ../bin/co65*),../bin/co65,co65)
LD = $(if $(wildcard ../bin/ld65*),../bin/ld65,ld65)

../wrk/$(TARGET)/%.o: %.s | ../wrk/$(TARGET)
	@echo $(TARGET) - $<
	@$(CA) -t $(TARGET) $(AFLAGS) --create-dep $(@:.o=.d) -o $@ $<

../wrk/$(TARGET)/%.o: %.c | ../wrk/$(TARGET)
	@echo $(TARGET) - $<
	@$(CC) -t $(TARGET) $(CFLAGS) --create-dep $(@:.o=.d) -o $(@:.o=.s) $<
	@$(CA) -t $(TARGET) -o $@ $(@:.o=.s)

../wrk/$(TARGET):
	@$(call MKDIR,$@)

$(EXTRA_OBJPAT): $(EXTRA_SRCPAT) | ../lib
	@echo $(TARGET) - $<
	@$(CA) -t $(TARGET) $(AFLAGS) -o $@ $<

../lib/$(TARGET).lib: $(OBJS) | ../lib
	$(AR) a $@ $?

../lib:
	@$(call MKDIR,$@)

$(TARGET): $(EXTRA_OBJS) ../lib/$(TARGET).lib

-include $(DEPS)

endif # TARGET
