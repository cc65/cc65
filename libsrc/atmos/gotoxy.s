;
; Ullrich von Bassewitz, 2003-04-13
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         _gotoxy
        .import         popa

        .include        "atmos.inc"

.proc   _gotoxy

        sta     CURS_Y          ; Set Y
        jsr     popa            ; Get X
        sta     CURS_X          ; Set X
        rts

.endproc


