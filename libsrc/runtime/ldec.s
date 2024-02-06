;
; Piotr Fusik, 09.03.2018
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Decrement eax by value in Y
;

        .export         deceaxy
        .importzp       sreg, tmp1

deceaxy:
        sty     tmp1
        sec
        sbc     tmp1
        bcs     @L9

; Borrow from X.

        dex
        cpx     #$FF
        bne     @L9

; X wrapped from zero to $FF, borrow from sreg.

        dec     sreg
        cpx     sreg
        bne     @L9

; sreg wrapped from zero to $FF, borrow from sreg+1.

        dec     sreg+1

; Done.

@L9:    rts

