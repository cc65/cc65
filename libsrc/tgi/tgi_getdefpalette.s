;
; Ullrich von Bassewitz, 23.06.2002
;
; const unsigned char* tgi_getdefpalette (void);
; /* Return the default palette. Will return NULL for drivers that do not
; ** support palettes.
; */
;

        .include        "tgi-kernel.inc"

_tgi_getdefpalette      =       tgi_getdefpalette       ; Call the driver
