;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_circle (int x, int y, unsigned char radius);
; /* Draw a circle in the current drawing color */

        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr1, ptr2, tmp1
        .export         _tgi_circle

_tgi_circle:
        sta     tmp1            ; Get the coordinates
        jsr     popax
        sta     ptr2
        stx     ptr2+1
        jsr     popax
        sta     ptr1
        stx     ptr1+1

        jmp     tgi_circle      ; Call the driver


