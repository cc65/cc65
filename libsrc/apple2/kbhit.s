;
; Kevin Ruland
; Ullrich von Bassewitz, 2005-03-25
;
; unsigned char kbhit (void);
;

        .export _kbhit

        .include "apple2.inc"

_kbhit:
        lda     KBD             ; Reading KBD checks for keypress
        rol                     ; if high bit is set, key was pressed
        lda     #$00
        tax
        rol
        rts
