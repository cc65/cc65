;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_outtext (const char* s);
; /* Output text at the current graphics cursor position. */


        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr3
        .export         _tgi_outtext

_tgi_outtext:
        sta     ptr3
        stx     ptr3+1          ; Save s
        jsr     tgi_curtoxy     ; Copy curx/cury into ptr1/ptr2
        jmp     tgi_outtext     ; Call the driver


