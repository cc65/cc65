;
; Marco van den Heuvel, 2018-04-25
;

; unsigned char __fastcall__ set_chameleon_speed (unsigned char speed);
;
;/* Set the speed of the Chameleon cartridge, the following inputs
; * are accepted:
; * SPEED_SLOW : 1 Mhz mode
; * SPEED_1X   : 1 Mhz mode
; * SPEED_2X   : 2 Mhz mode
; * SPEED_3X   : 3 Mhz mode
; * SPEED_4X   : 4 Mhz mode
; * SPEED_5X   : 5 Mhz mode
; * SPEED_6X   : 6 Mhz mode
; * SPEED_FAST : Maximum speed mode
; *
; * Note that any value higher or equal to SPEED_7X will switch to maximum
; * speed mode.
; *
; * This function will return the actual speed the CPU is at after trying
; * to set the requested speed, to my knowledge the switching should not fail.
; *
; * This function does not check for the presence of the Chameleon cartridge,
; * make sure you use 'detect_chameleon();' before using.
; */

; unsigned char get_chameleon_speed (void);
;
;/* Get the speed of the Chameleon cartridge.
; *
; * Possible return values:
; * SPEED_SLOW  : Slow mode
; * SPEED_2X    : 2Mhz mode
; * SPEED_3X    : 3Mhz mode
; * SPEED_4X    : 4Mhz mode
; * SPEED_5X    : 5Mhz mode
; * SPEED_6X    : 6Mhz mode
; * SPEED_FAST  : Maximum speed mode
; *
; * This function does not check for the presence of the Chameleon cartridge,
; * make sure you use 'detect_chameleon();' before using.
; */

        .export         _set_chameleon_speed
        .export         _get_chameleon_speed

        .include        "accelerator.inc"

_set_chameleon_speed:
        cmp     #SPEED_7X
        bcs     maximum_speed
        cmp     #SPEED_1X
        beq     low_speed
        ora     #$80
set_speed:
        jsr     activate_regs
        sta     CHAMELEON_CFGTUR
        jmp     return_speed

low_speed:
        lda     #CHAMELEON_CFGTUR_LIMIT_1MHZ
        bne     set_speed

maximum_speed:
        lda     #CHAMELEON_CFGTUR_LIMIT_NONE
        bne     set_speed

_get_chameleon_speed:
        jsr     activate_regs
return_speed:
        ldx     #$00
        lda     CHAMELEON_CFGTUR
        tay
        and     #%10000000
        beq     return_value
        tya
        and     #%00001000
        bne     is_slow_mode
        tya
        and     #%00000111
        beq     is_max_mode
return_value:
        ldy     #CHAMELEON_DISABLE_REGS
        sty     CHAMELEON_CFGENA
        rts

is_slow_mode:
        txa
        bne     return_value

is_max_mode:
        lda     #SPEED_FAST
        bne     return_value

activate_regs:
        ldy     #CHAMELEON_ENABLE_REGS
        sty     CHAMELEON_CFGENA
        rts

