;
; Ullrich von Bassewitz, 06.08.1998
; Brandon Woodward, 2026-09-14
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy
        .import         setcursor_posx
        .import         popa

        .include        "nes.inc"

gotoxy:
        jsr     popa            ; Get Y

_gotoxy:
        sta     CURS_Y          ; Set Y
        jsr     popa            ; Get X
        tay
        ldx     CURS_Y
        jmp     setcursor_posx  ; Set X
