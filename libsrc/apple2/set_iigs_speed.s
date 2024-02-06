;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; unsigned char __fastcall__ detect_iigs(unsigned char speed)
;

        .export         _set_iigs_speed
        .import         ostype, return0

        .include        "apple2.inc"
        .include        "accelerator.inc"

_set_iigs_speed:
        tax                     ; Keep parameter
        lda     ostype          ; Return if not IIgs
        bmi     :+
        jmp     return0

:       lda     CYAREG
        cpx     #SPEED_SLOW
        beq     :+
        ora     #%10000000
        bne     set_speed
:       and     #%01111111
set_speed:
        sta     CYAREG
        txa
        ldx     #$00
        rts
