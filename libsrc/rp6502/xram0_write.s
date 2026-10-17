;
; 2026, Rumbledethumps
;
; void __fastcall__ xram0_write (unsigned dest, const void* src, unsigned count);

.include "xram.inc"

.export _xram0_write

.code

_xram0_write:
        xram_write RIA_RW0, RIA_ADDR0, RIA_STEP0
