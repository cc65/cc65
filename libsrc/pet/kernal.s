;
; Ullrich von Bassewitz, 19.11.2002
;
; PET kernal functions
;

        .export         CLRCH
        .export         BASIN
        .export         STOP
        .export         GETIN
        .export         CLALL
        .export         UDTIM






;-----------------------------------------------------------------------------
; Functions that are available in the kernal jump table

CLRCH           = $FFCC
BASIN           = $FFCF
STOP            = $FFE1
GETIN           = $FFE4
CLALL           = $FFE7
UDTIM           = $FFEA

