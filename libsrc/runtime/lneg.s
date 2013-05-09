;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: negation on longs
;

;
; eax = -eax
;
        .export         negeax
        .importzp       sreg

negeax: clc
        eor     #$FF
        adc     #1
        pha
        txa
        eor     #$FF
        adc     #0
        tax
        lda     sreg
        eor     #$FF
        adc     #0
        sta     sreg
        lda     sreg+1
        eor     #$FF
        adc     #0
        sta     sreg+1
        pla
        rts

