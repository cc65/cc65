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

        ; Ensure DCSEL=0
        lda     VERA::CTRL
        and     #$01
        sta     VERA::CTRL

        lda     VERA::DC_BORDER             ; get old value
        stx     VERA::DC_BORDER             ; set new value
        rts
