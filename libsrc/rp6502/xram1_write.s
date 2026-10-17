;
; 2026, Rumbledethumps
;
; void __fastcall__ xram1_write (unsigned dest, const void* src, unsigned count);

.include "xram.inc"

.export _xram1_write

.code

_xram1_write:
        xram_write RIA_RW1, RIA_ADDR1, RIA_STEP1
