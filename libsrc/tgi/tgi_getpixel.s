;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned char __fastcall__ tgi_getpixel (int x, int y);
; /* Get the color value of a pixel */


        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr1, ptr2
        .export         _tgi_getpixel

_tgi_getpixel:
        sta     ptr2            ; Get the coordinates
        stx     ptr2+1
        jsr     popax
        sta     ptr1
        stx     ptr1+1

        jmp     tgi_getpixel    ; Call the driver



