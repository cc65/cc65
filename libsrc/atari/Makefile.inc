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

CFLAGS += -Wa -DNUMDRVS=4,-DLINEBUF=80,-DUCASE_FILENAME=1,-DDEFAULT_DEVICE=1,-DDYNAMIC_DD=1
