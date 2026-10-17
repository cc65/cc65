;
; 2026, Rumbledethumps
;
; void __fastcall__ _xram1_poke8 (unsigned char val, unsigned addr);

.include "xram.inc"

.export __xram1_poke8

.code

__xram1_poke8:
        xram_poke8 RIA_RW1, RIA_ADDR1
