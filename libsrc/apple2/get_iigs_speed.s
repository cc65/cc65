;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; unsigned char __fastcall__ get_iigs_speed(void)
;

        .export         _get_iigs_speed
        .import         ostype, return0

        .include        "apple2.inc"
        .include        "accelerator.inc"

_get_iigs_speed:
        lda     ostype          ; Return SLOW if not IIgs
        bpl     :+
        lda     CYAREG          ; Check current setting
        bpl     :+
        lda     #SPEED_FAST
        ldx     #$00
        rts
        .assert SPEED_SLOW = 0, error
:       jmp     return0         ; SPEED_SLOW
