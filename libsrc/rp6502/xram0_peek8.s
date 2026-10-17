;
; 2026, Rumbledethumps
;
; unsigned char __fastcall__ xram0_peek8 (unsigned addr);

.include "xram.inc"

.export _xram0_peek8

.code

_xram0_peek8:
        xram_peek8 RIA_RW0, RIA_ADDR0
