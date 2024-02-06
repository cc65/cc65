;
; 2020-05-02, Greg King
;
; unsigned char __fastcall__ bordercolor (unsigned char color);
; /* Set the color for the border. The old color setting is returned. */
;

        .export         _bordercolor

        .include        "cx16.inc"

_bordercolor:
        tax
        stz     VERA::CTRL              ; Use display register bank 0
        lda     VERA::DISP::FRAME       ; get old value
        stx     VERA::DISP::FRAME       ; set new value
        rts
