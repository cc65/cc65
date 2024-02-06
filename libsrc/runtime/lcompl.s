;
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: long complement
;

        .export         compleax
        .importzp       sreg

; eax = ~eax

compleax:
        eor     #$FF
        pha
        txa
        eor     #$FF
        tax
        lda     sreg
        eor     #$FF
        sta     sreg
        lda     sreg+1
        eor     #$FF
        sta     sreg+1
        pla
        rts

