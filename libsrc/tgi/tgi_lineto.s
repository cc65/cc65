;
; Ullrich von Bassewitz, 22.06.2002
;
; void __fastcall__ tgi_lineto (int x2, int y2);
; /* Draw a line in the current drawing color from the graphics cursor to the
;  * new end point.
;  */

        .include        "tgi-kernel.inc"

        .import         popax
        .export         _tgi_lineto

_tgi_lineto:
        jsr     tgi_curtoxy     ; Copy curx/cury into ptr1/ptr2
        jsr     tgi_linepop     ; Pop x2/y2 into ptr3/ptr4 and curx/cury
        jmp     tgi_line        ; Call the driver


