;
; 2026, Rumbledethumps
;
; void __fastcall__ xram1_set (unsigned dest, unsigned char val, unsigned count);

.include "xram.inc"

.export _xram1_set

.code

_xram1_set:
        xram_set RIA_RW1, RIA_ADDR1, RIA_STEP1
