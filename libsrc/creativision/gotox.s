;
; Ullrich von Bassewitz, 2003-05-02
;
; void gotox (unsigned char x);
;

        .export         _gotox
        .import         setcursor

        .include        "creativision.inc"

.proc   _gotox

        sta     CURSOR_X        ; Set new position
        tay
        ldx     CURSOR_Y
        jmp     setcursor       ; Set the cursor to the new position

.endproc
