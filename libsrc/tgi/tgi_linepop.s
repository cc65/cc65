;
; Ullrich von Bassewitz, 22.06.2002
;
; Helper function for tgi_line and tgi_lineto. Pops/stores X2/Y2.
;

        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr3, ptr4

tgi_linepop:
        sta     ptr4            ; Y2
        stx     ptr4+1
        sta     _tgi_cury
        stx     _tgi_cury+1
        jsr     popax
        sta     ptr3            ; X2
        stx     ptr3+1
        sta     _tgi_curx
        sta     _tgi_curx+1
        rts


