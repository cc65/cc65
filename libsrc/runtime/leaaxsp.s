;
; Ullrich von Bassewitz, 2009-08-31
;
; CC65 runtime: Load effective address with offset in A/X relative to SP
;

        .export         leaaxsp, leaa0sp
        .importzp       spc

leaa0sp:
        ldx     #$00
leaaxsp:
        clc
        adc     spc
        pha
        txa
        adc     spc+1
        tax
        pla
        rts



