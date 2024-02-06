# NUMDRVS        - number of supported drives (max. 16)
#                  4 bytes for each device are statically allocated
# LINEBUF        - support line buffered reads from E: (the number specifies
#                  the length of the buffer)
# UCASE_FILENAME - all filenames get uppercased
# DEFAULT_DEVICE - if the string passed to the uppercase function doesn't
#                  include a device (":" at position 2 or 3), provide "Dn:"
#                  as a default disk device
#                  n is the value of DEFAULT_DEVICE, unless DYNAMIC_DD is
#                  also set, in which case it's overridden by a runtime
#                  check (on SpartaDOS only)
#                  needs UCASE_FILENAME to be defined, otherwise no effect
# DYNAMIC_DD     - determine default disk device at runtime (SpartaDOS only)
#                  needs DEFAULT_DEVICE to be defined, otherwise no effect

CA65FLAGS += -D NUMDRVS=4 -D LINEBUF=80 -D UCASE_FILENAME=1 -D DEFAULT_DEVICE=1 -D DYNAMIC_DD=1

# The following defines are only used by the 'atarixl' target:
#
# CHARGEN_RELOC  - If defined, CHBAS and CHBASE are updated when
#                  enabling or disabling the ROM.
#                  If the ROM is enabled, $E0 is written to CHBAS
#                  and CHBASE.
#                  If the ROM is disabled, the upper byte of
#                  __CHARGEN_START__ is written to CHBAS and CHBASE.
# USEWSYNC       - If defined, the code waits for horizontal retrace
#                  before switching the ROM and updating CHBAS and
#                  CHBASE. This define only has effect if CHARGEN_RELOC
#                  is also defined.

# Disabled by default, you should enable it if the linker script relocates the
# character generator (like atarixl-largehimem.cfg).
#CA65FLAGS += -D CHARGEN_RELOC -D USEWSYNC

# Disable if you don't want to use page 6 for mouse P/M data.
# If disabled, top of the RAM is used for P/M data.
CA65FLAGS += -D USE_PAGE6
