;
; Ullrich von Bassewitz, 2003-04-13
;
; void gotox (unsigned char x);
;

        .export         _gotox

        .include        "atmos.inc"

.proc   _gotox

        sta     CURS_X          ; Set X
        rts

.endproc


