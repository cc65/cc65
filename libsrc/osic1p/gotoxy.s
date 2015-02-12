;
; copied from CBM implementation
;
; originally by:
; Ullrich von Bassewitz, 06.08.1998
;
; void gotoxy (unsigned char x, unsigned char y);
;
        .export         _gotoxy
        .import         popa, plot
        .include        "extzp.inc"

_gotoxy:
        sta     CURS_Y          ; Set Y
        jsr     popa            ; Get X
        sta     CURS_X          ; Set X
        jmp     plot            ; Set the cursor position
