;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: negation on ints
;

        .export         negax

negax:  clc
        eor     #$FF
        adc     #1
        pha
        txa
        eor     #$FF
        adc     #0
        tax
        pla
        rts



