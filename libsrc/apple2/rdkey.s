;
; Oliver Schmidt, 03.03.2007
;
; RDKEY routine
;

        .export         RDKEY

        .include        "apple2.inc"

        .segment        "LOWCODE"

RDKEY:
        ; Switch in ROM and call RDKEY
        bit     $C082
        jsr     $FD0C           ; Display prompt and read key from user input routine

        ; Switch in LC bank 2 for R/O and return
        bit     $C080
        rts
