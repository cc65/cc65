;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_circle (int x, int y, unsigned char radius);
; /* Draw a circle in the current drawing color */

        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       tmp1

.proc   _tgi_circle

        sta     tmp1            ; Get the coordinates
        jsr     popax
        jsr     tgi_popxy       ; Pop X/Y into ptr1/ptr2
        jmp     tgi_circle      ; Call the driver

.endproc
