;
; 2026, Rumbledethumps
;
; void __fastcall__ ria_irq_write (unsigned char mask);

.include "rp6502.inc"

.export _ria_irq_write

.code

_ria_irq_write:
        sta     RIA_IRQ
        rts
