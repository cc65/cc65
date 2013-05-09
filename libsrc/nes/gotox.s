;
; Ullrich von Bassewitz, 2003-05-02
;
; void gotox (unsigned char x);
;

        .export         _gotox
        .import         setcursor

        .include        "nes.inc"

.proc   _gotox

        sta     CURS_X          ; Set new position
        tay
        ldx     CURS_Y
        jmp     setcursor       ; Set the cursor to the new position

.endproc


