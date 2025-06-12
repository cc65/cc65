;
; Eric Badger adeapted from Apple II version by Kevin Ruland
;
; char cgetc (void);
;
;

        .export         _cgetc
        .import         cursor, putchardirect
        .include "ebadger.inc"
_cgetc:
        lda LASTCHAR
        cmp #$00
        bne :+
        jsr READCHARUPPER
:
        and #$7F
        tax
        lda #$00
        sta LASTCHAR
        txa
        ldx #$00
        rts
