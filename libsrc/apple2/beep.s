;
; Colin Leroy-Mira, 2024
;
; void beep(void)
;

        .export         _beep
        .import         BELL

        .include        "apple2.inc"

        .segment        "LOWCODE"

_beep:
        lda     CH              ; Bell scrambles CH in 80col mode on IIgs, storing
        pha                     ; it in OURCH and resetting CH to 0. Save it.
        jsr     BELL
        pla
        sta     CH              ; Restore CH
        rts
