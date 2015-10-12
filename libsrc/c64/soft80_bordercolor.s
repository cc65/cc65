;
; Groepaz/Hitmen, 12.10.2015
;
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

        .export         soft80_bordercolor

        .include        "c64.inc"

        ; FIXME: if we'd move this function into a seperate file in the regular
        ;        conio lib, then we dont need this override at all.
soft80_bordercolor:
        ldx     VIC_BORDERCOLOR         ; get old value
        sta     VIC_BORDERCOLOR         ; set new value
        txa
        rts
