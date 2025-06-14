;
; Ullrich von Bassewitz, 2009-08-31
;
; CC65 runtime: Load effective address with offset in A/X relative to SP
;

        .export         leaaxsp, leaa0sp
        .importzp       c_sp

leaa0sp:
        ldx     #$00
leaaxsp:
        clc
        adc     c_sp
        pha
        txa
        adc     c_sp+1
        tax
        pla
        rts



