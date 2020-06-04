;
; 2002-06-21, Ullrich von Bassewitz
; 2020-06-04, Greg King
;
; void __fastcall__ tgi_setcolor (unsigned char color);
; /* Set the current drawing color */


        .include        "tgi-kernel.inc"

.proc   _tgi_setcolor

        cmp     _tgi_colorcount ; Compare to available colors
        bcs     @L1
@L0:    sta     _tgi_color      ; Remember the drawing color
        jmp     tgi_setcolor    ; Call the driver

@L1:    ldx     _tgi_colorcount
        beq     @L0             ; Zero means 256 colors
        jmp     tgi_inv_arg     ; Invalid argument

.endproc
