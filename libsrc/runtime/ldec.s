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
        dex
        cpx     #$ff
        bne     @L9
        dec     sreg
        cpx     sreg
        bne     @L9
        dec     sreg+1
@L9:    rts

