;
; Kevin Ruland
; Ullrich von Bassewitz, 2005-03-25
; Oleg A. Odintsov, Moscow, 2024
;
; unsigned char kbhit (void);
;

        .export _kbhit

        .include "agat.inc"

_kbhit:
        lda     KBD             ; Reading KBD checks for keypress
        rol                     ; if high bit is set, key was pressed
        lda     #$00
        tax
        rol
        rts
