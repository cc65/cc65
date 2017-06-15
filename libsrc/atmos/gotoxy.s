;
; 2003-04-13, Ullrich von Bassewitz
; 2017-06-15, Greg King
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy

        .import         popa

        .include        "atmos.inc"

gotoxy: jsr     popa            ; Get Y

.proc   _gotoxy

        sta     CURS_Y          ; Set Y
        jsr     popa            ; Get X
        sta     CURS_X          ; Set X
        rts

.endproc
