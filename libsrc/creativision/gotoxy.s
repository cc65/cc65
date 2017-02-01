;
; Ullrich von Bassewitz, 06.08.1998
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         _gotoxy
        .import         setcursor
        .import         popa

        .include        "creativision.inc"

.proc   _gotoxy

        sta     CURSOR_Y        ; Set Y
        jsr     popa            ; Get X
        sta     CURSOR_X        ; Set X
        tay
        ldx     CURSOR_Y
        jmp     setcursor       ; Set the cursor position

.endproc
