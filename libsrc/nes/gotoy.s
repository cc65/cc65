;
; Ullrich von Bassewitz, 2003-05-02
;
; void gotoy (unsigned char y);
;

        .export         _gotoy
        .import         setcursor

        .include        "nes.inc"

.proc   _gotoy

        sta     CURS_Y          ; Set new position
        tax
        ldy     CURS_X
        jmp     setcursor       ; Set the cursor to the new position

.endproc


              
