;
; Christian Groessler, 02-Apr-2019
;

        .export         _bgcolor

        .include        "atari.inc"


_bgcolor:
        ldx     COLOR2  ; get old value
        sta     COLOR2  ; set new value
        and     #$0e
        cmp     #8
        bcs     bright
        lda     #$0e
        .byte   $2c     ; bit opcode, eats the next 2 bytes
bright: lda     #0
        sta     COLOR1
        txa
        ldx     #0      ; fix X
        rts
