;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_line (int x1, int y1, int x2, int y2);
; /* Draw a line in the current drawing color */


        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr1, ptr2, ptr3, ptr4
        .export         _tgi_line

_tgi_line:
        sta     ptr4            ; Get the coordinates
        stx     ptr4+1
        jsr     popax
        sta     ptr3
        stx     ptr3+1
        jsr     popax
        sta     ptr2
        stx     ptr2+1
        jsr     popax
        sta     ptr1
        stx     ptr1+1

        jmp     tgi_line        ; Call the driver


