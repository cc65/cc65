;
; Colin Leroy-Mira, 2024
;
; BELL routine
;

        .export         BELL

        .include        "apple2.inc"

        .segment        "LOWCODE"

BELL:
        ; Switch in ROM and call BELL
        bit     $C082
        jsr     $FF3A           ; BELL

        ; Switch in LC bank 2 for R/O and return
        bit     $C080
        rts
