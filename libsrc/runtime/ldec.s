;
; Piotr Fusik, 07.03.2018
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
; borrow from X
        dex
        cpx     #$ff
        bne     @L9
; X wrapped from zero to $ff, borrow from sreg
        dec     sreg
        cpx     sreg
        bne     @L9
; sreg wrapped from zero to $ff, borrow from sreg+1
        dec     sreg+1
@L9:    rts

