;
; Ullrich von Bassewitz, 22.06.2002
;
; void __fastcall__ tgi_gotoxy (int x, int y);
; /* Set the current drawing pointer to the given position. */
;

        .include        "tgi-kernel.inc"

        .import         popax

.proc   _tgi_gotoxy

        sta     _tgi_cury               ; Y
        stx     _tgi_cury+1
        jsr     popax
        sta     _tgi_curx               ; X
        stx     _tgi_curx+1
        rts

.endproc

