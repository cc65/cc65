;
; Ullrich von Bassewitz, 06.08.1998
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .include "atari.inc"

        .export         gotoxy, _gotoxy
        .import         popa
        .import         setcursor

gotoxy:
        jsr     popa            ; Get Y

_gotoxy:                        ; Set the cursor position
        sta     ROWCRS          ; Set Y
        jsr     popa            ; Get X
        sta     COLCRS          ; Set X
        lda     #0
        sta     COLCRS+1
        jmp     setcursor
