;
; 2026, Rumbledethumps
;
; unsigned char __fastcall__ xram1_peek8 (unsigned addr);

.include "xram.inc"

.export _xram1_peek8

.code

_xram1_peek8:
        xram_peek8 RIA_RW1, RIA_ADDR1
