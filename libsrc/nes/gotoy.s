;
; Ullrich von Bassewitz, 2003-05-02
; Brandon Woodward, 2026-09-14
;
; void gotoy (unsigned char y);
;

        .export         _gotoy
        .import         setcursor_pos

        .include        "nes.inc"

.proc   _gotoy

        tax
        ldy     CURS_X
        jmp     setcursor_pos   ; Set the cursor to the new position

.endproc
