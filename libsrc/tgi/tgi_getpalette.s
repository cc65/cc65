;
; Ullrich von Bassewitz, 23.06.2002
;
; const unsigned char* __fastcall__ tgi_getpalette (void);
; /* Return the current palette. Will return NULL for drivers that do not
;  * support palettes.
;  */
;

        .include        "tgi-kernel.inc"
        .export         _tgi_getpalette


_tgi_getpalette         =       tgi_getpalette          ; Call the driver



