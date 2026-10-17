;
; 2026, Rumbledethumps
;
; unsigned char ria_irq_read (void);

.include "rp6502.inc"

.export _ria_irq_read

.code

_ria_irq_read:
        lda     RIA_IRQ
        ldx     #0
        rts
