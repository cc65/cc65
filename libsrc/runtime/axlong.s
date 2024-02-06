;
; Ullrich von Bassewitz, 25.10.2000
; Christian Krueger, 02-Mar-2017, some bytes saved
;
; CC65 runtime: Convert int in ax into a long
;

        .export         axulong, axlong
        .importzp       sreg

; Convert AX from int to long in EAX

axlong: ldy     #$ff
        cpx     #$80            ; Positive?
        bcs     store           ; No, apply $FF

axulong:
        ldy     #0
store:  sty     sreg
        sty     sreg+1
        rts
