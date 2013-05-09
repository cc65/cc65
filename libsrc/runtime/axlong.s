;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Convert int in ax into a long
;

        .export         axulong, axlong
        .importzp       sreg

; Convert AX from int to long in EAX

axulong:
        ldy     #0
        sty     sreg
        sty     sreg+1
        rts

axlong: cpx     #$80            ; Positive?
        bcc     axulong         ; Yes, handle like unsigned type
        ldy     #$ff
        sty     sreg
        sty     sreg+1
        rts



