;
; 1998-08-06, Ullrich von Bassewitz
; 2017-06-15, Greg King
;
; void gotoxy (unsigned char x, unsigned char y);
;

        .export         gotoxy, _gotoxy

        .import         setcursor
        .import         popa

        .include        "creativision.inc"

gotoxy: jsr     popa            ; Get Y

.proc   _gotoxy

        sta     CURSOR_Y        ; Set Y
        jsr     popa            ; Get X
        sta     CURSOR_X        ; Set X
        tay
        ldx     CURSOR_Y
        jmp     setcursor       ; Set the cursor position

.endproc
