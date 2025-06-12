;
; Ullrich von Bassewitz, 2005-03-28
;
; unsigned char __fastcall__ revers (unsigned char onoff)
;

        .export         _revers

        .include        "ebadger.inc"

_revers:
        lda #$00
        rts
