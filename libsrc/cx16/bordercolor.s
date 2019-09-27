;
; 2019-09-23, Greg King
;
; unsigned char __fastcall__ bordercolor (unsigned char color);
; /* Set the color for the border. The old color setting is returned. */
;

        .export         _bordercolor

        .include        "cx16.inc"

_bordercolor:
        tax

        ; Point to the border color register.

        stz     VERA::CTRL              ; Use port 0
        lda     #<VERA::COMPOSER::FRAME
        sta     VERA::ADDR
        lda     #>VERA::COMPOSER::FRAME
        sta     VERA::ADDR+1
        ldy     #^VERA::COMPOSER::FRAME | VERA::INC0
        sty     VERA::ADDR+2

        lda     VERA::DATA0             ; get old value
        stx     VERA::DATA0             ; set new value
        rts
