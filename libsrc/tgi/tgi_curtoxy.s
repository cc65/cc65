;
; Ullrich von Bassewitz, 02.10.2002
;
; Helper function for tgi functions. Moves the current X/Y pos to ptr1/ptr2
;

        .include        "tgi-kernel.inc"

        .importzp       ptr1, ptr2

tgi_curtoxy:
        ldy     _tgi_curx       ; X1
        sty     ptr1
        ldy     _tgi_curx+1
        sty     ptr1+1

        ldy     _tgi_cury       ; Y1
        sty     ptr2
        ldy     _tgi_cury+1
        sty     ptr2+1
        rts

