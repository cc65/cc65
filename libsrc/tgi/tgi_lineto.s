;
; Ullrich von Bassewitz, 22.06.2002
;
; void __fastcall__ tgi_lineto (int x2, int y2);
; /* Draw a line in the current drawing color from the graphics cursor to the
;  * new end point.
;  */

        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr1, ptr2, ptr3, ptr4
        .export         _tgi_lineto

_tgi_lineto:
        ldy     _tgi_curx       ; X1
        sty     ptr1
        ldy     _tgi_curx+1
        sty     ptr1+1

        ldy     _tgi_cury       ; Y1
        sty     ptr2
        ldy     _tgi_cury+1
        sty     ptr2+1

        jsr     tgi_linepop

        jmp     tgi_line        ; Call the driver


