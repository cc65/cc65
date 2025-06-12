;
; Colin Leroy-Mira, 2024
;
; void beep(void)
;

        .export         _beep

        .include        "apple2.inc"

        .segment        "LOWCODE"

_beep:
        ; Switch in ROM and call BELL
        bit     $C082
        jsr     $FF3A           ; BELL

        ; Switch in LC bank 2 for R/O and return
        bit     $C080
        rts
