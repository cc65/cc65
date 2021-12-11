;
; Ullrich von Bassewitz, 05.08.1998
;
; int abs (int x);
; and
; CC65 runtime: negation on ints
;

        .export         negax
        .export         _abs

_abs:   cpx     #$00            ; Test hi byte
        bpl     L1              ; Don't touch if positive
negax:  clc
        eor     #$FF
        adc     #1
        pha
        txa
        eor     #$FF
        adc     #0
        tax
        pla
 L1:    rts



