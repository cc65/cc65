;
; Ullrich von Bassewitz, 19.11.2002
;
; PET kernal functions
;

        .export         OPEN
        .export         CLOSE
        .export         CHKIN
        .export         CKOUT
        .export         CLRCH
        .export         BASIN
        .export         BSOUT
        .export         STOP
        .export         GETIN
        .export         CLALL
        .export         UDTIM






;-----------------------------------------------------------------------------
; Functions that are available in the kernal jump table

OPEN		= $FFC0
CLOSE		= $FFC3
CHKIN 		= $FFC6
CKOUT 		= $FFC9
CLRCH 		= $FFCC
BASIN 		= $FFCF
BSOUT 		= $FFD2
STOP  		= $FFE1
GETIN 		= $FFE4
CLALL 		= $FFE7
UDTIM 		= $FFEA

