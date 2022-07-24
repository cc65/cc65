;
; Ullrich von Bassewitz, 06.08.1998
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy
        .import         setcursor
        .import         popa

        .include        "nes.inc"

gotoxy:
        jsr     popa            ; Get Y

_gotoxy:
        sta     CURS_Y          ; Set Y
        jsr     popa            ; Get X
        sta     CURS_X          ; Set X
        tay
        ldx     CURS_Y
        jmp     setcursor       ; Set the cursor position
