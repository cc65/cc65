;
; void __fastcall__ gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy

        .import         popa, plot

        .include        "pce.inc"
        .include        "extzp.inc"

gotoxy:
        jsr     popa            ; Get Y

_gotoxy:
        sta     CURS_Y          ; Set Y
        jsr     popa            ; Get X
        sta     CURS_X          ; Set X
        jmp     plot            ; Set the cursor position
