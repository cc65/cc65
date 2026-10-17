;
; 2026, Rumbledethumps
;
; void __fastcall__ _xram0_poke8 (unsigned char val, unsigned addr);

.include "xram.inc"

.export __xram0_poke8

.code

__xram0_poke8:
        xram_poke8 RIA_RW0, RIA_ADDR0
