;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_setpixel (int x, int y);
; /* Plot a point in the current drawing color */


        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr1, ptr2
        .export         _tgi_setpixel

_tgi_setpixel:
        sta     ptr2            ; Get the coordinates
        stx     ptr2+1
        jsr     popax
        sta     ptr1
        stx     ptr1+1

        jmp     tgi_setpixel    ; Call the driver

