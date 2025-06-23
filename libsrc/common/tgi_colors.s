;
; Target-specific black & white values for use by the target-shared TGI kernel
;
; NOTE: These are indices into the default palette

        .include        "tgi-kernel.inc"

        .export         tgi_color_black:zp = $00
        .export         tgi_color_white:zp = $01
