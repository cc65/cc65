;
; Ullrich von Bassewitz, 2003-02-13
;
; void c64mode (void);
; /* Switch the C128 into C64 mode. Note: This function will not return! */
;

        .export         _c64mode
        .import         C64MODE

        _c64mode        = C64MODE

