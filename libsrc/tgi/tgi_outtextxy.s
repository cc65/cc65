;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_outtextxy (int x, int y, const char* s);
; /* Output text at the given position. */


        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr3
        .export         _tgi_outtextxy

_tgi_outtextxy:
        sta     ptr3
        stx     ptr3+1          ; Save s
        jsr     tgi_popxy       ; Pop x/y into ptr1/ptr2
        jmp     tgi_outtext     ; Call the driver


