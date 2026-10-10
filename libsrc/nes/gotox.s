;
; Ullrich von Bassewitz, 2003-05-02
; Brandon Woodward, 2026-09-14
;
; void gotox (unsigned char x);
;

        .export         _gotox
        .import         setcursor_posx

        .include        "nes.inc"

.proc   _gotox

        tay
        ldx     CURS_Y
        jmp     setcursor_posx  ; Set the cursor to the new X position

.endproc
