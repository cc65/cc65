;
; Ullrich von Bassewitz, 23.06.2002
;
; const unsigned char* __fastcall__ tgi_getdefpalette (void);
; /* Return the default palette. Will return NULL for drivers that do not
;  * support palettes.
;  */
;

        .include        "tgi-kernel.inc"
        .export         _tgi_getdefpalette

_tgi_getdefpalette      =       tgi_getdefpalette       ; Call the driver


