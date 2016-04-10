;
; Ullrich von Bassewitz, 2003-05-02
;
; void gotoy (unsigned char y);
;

        .export         _gotoy
        .import         setcursor

        .include        "creativision.inc"

.proc   _gotoy

        sta     CURSOR_Y        ; Set new position
        tax
        ldy     CURSOR_X
        jmp     setcursor       ; Set the cursor to the new position

.endproc
