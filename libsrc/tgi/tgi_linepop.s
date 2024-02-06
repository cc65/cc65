;
; Ullrich von Bassewitz, 22.06.2002
;
; Helper function for tgi_line and tgi_lineto. Pops/stores X2/Y2.
;

        .include        "tgi-kernel.inc"

        .import         popax

.proc   tgi_linepop

        sta     tgi_clip_y2     ; Y2
        stx     tgi_clip_y2+1
        sta     _tgi_cury
        stx     _tgi_cury+1

        jsr     popax

        sta     tgi_clip_x2     ; X2
        stx     tgi_clip_x2+1
        sta     _tgi_curx
        stx     _tgi_curx+1
        rts

.endproc

