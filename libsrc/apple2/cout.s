;
; Oliver Schmidt, 03.03.2007
;
; COUT routine
;

        .export         COUT

        .include        "apple2.inc"

        .segment        "LOWCODE"

COUT:
        ; Switch in ROM and call COUT
        bit     $C082
        jsr     $FDED           ; Vector to user output routine

        ; Switch in LC bank 2 for R/O and return
        bit     $C080
        rts
