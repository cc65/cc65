;
; Ullrich von Bassewitz, 2003-04-13
;
; void gotoy (unsigned char y);
;

        .export         _gotoy

        .include        "atmos.inc"

.proc   _gotoy

        sta     CURS_Y          ; Set Y
        rts

.endproc


