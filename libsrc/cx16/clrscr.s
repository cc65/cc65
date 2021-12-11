;
; 2019-09-22, Greg King
;
; void clrscr (void);
; /* Clear the screen. */
;

        .export         _clrscr

        .import         CHROUT

        .include        "cx16.inc"


; An internal Kernal function can't be used because it might be moved in future
; revisions.  Use an official function; but, make sure that it prints
; to the screen.

_clrscr:
        ldy     OUT_DEV         ; Save current output device
        ldx     #$03            ; Screen device
        stx     OUT_DEV
        lda     #$93
        jsr     CHROUT          ; Print clear-screen character
        sty     OUT_DEV         ; Restore output device
        rts
