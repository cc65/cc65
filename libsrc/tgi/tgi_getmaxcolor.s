;
; Ullrich von Bassewitz, 22.06.2002
;
; unsigned char __fastcall__ tgi_getmaxcolor (void);
; /* Return the maximum supported color number (the number of colors would
;  * then be getmaxcolor()+1).
;  */

        .include        "tgi-kernel.inc"
        .export         _tgi_getmaxcolor


_tgi_getmaxcolor:
        ldx     _tgi_colors
        dex
        txa
        ldx     #0
        rts



