;
; Colin Leroy-Mira, 2024
;
; WAIT routine
;

        .export         WAIT

        .include        "apple2.inc"

        .segment        "LOWCODE"

WAIT:
        ; Switch in ROM and call WAIT
        bit     $C082
        jsr     $FCA8           ; Vector to WAIT routine

        ; Switch in LC bank 2 for R/O and return
        bit     $C080
        rts
