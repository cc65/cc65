;
; Christian Groessler, 13-Mar-2014
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .include "atari5200.inc"

        .export         gotoxy, _gotoxy
        .import         popa

gotoxy:
        jsr     popa            ; Get Y

_gotoxy:                        ; Set the cursor position
        sta     ROWCRS_5200     ; Set Y
        jsr     popa            ; Get X
        sta     COLCRS_5200     ; Set X
        rts
