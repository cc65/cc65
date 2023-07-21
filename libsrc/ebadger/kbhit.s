;
; Eric Badger
; Kevin Ruland
; Ullrich von Bassewitz, 2005-03-25
;
; unsigned char kbhit (void);
;

        .export _kbhit
        .include "ebadger.inc"

_kbhit:
        lda #$00
        jsr READCHARASYNC
        cmp #$00
        beq done
        lda #$1
done:
        rts
