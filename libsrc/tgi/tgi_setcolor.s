;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_setcolor (unsigned char color);
; /* Set the current drawing color */


        .include        "tgi-kernel.inc"

.proc   _tgi_setcolor

        cmp     _tgi_colorcount ; Compare to available colors
        bcs     @L1
        sta     _tgi_color      ; Remember the drawing color
        jmp     tgi_setcolor    ; Call the driver
@L1:    jmp     tgi_inv_arg     ; Invalid argument

.endproc

