;
; Marco van den Heuvel, 2018-04-30
;

; unsigned char __fastcall__ set_turbomaster_speed (unsigned char speed);
;
;/* Set the speed of the Turbo Master cartridge, using SPEED_SLOW will switch to
; * 1 Mhz mode, SPEED_4X or SPEED_FAST will switch to 4 Mhz mode.
; *
; * Note that any value higher or equal to SPEED_4X will switch to 4 Mhz mode,
; * any value lower than SPEED_4X will switch to 1 Mhz mode.
; *
; * This function will return the actual speed the CPU is at after trying
; * to set the requested speed, if the speed is different it might indicate
; * that the hardware switch has locked the speed.
; *
; * This function does not check for the presence of a Turbo Master cartridge,
; * make sure you use 'detect_turbomaster();' before using.
; */

; unsigned char get_turbomaster_speed (void);
;
;/* Get the speed of the Turbo Master cartridge.
; *
; * Possible return values:
; * SPEED_SLOW  : 1 Mhz mode
; * SPEED_4X    : 4 Mhz mode
; *
; * This function does not check for the presence of a Turbo Master cartridge,
; * make sure you use 'detect_turbomaster();' before using.
; */

        .export         _set_turbomaster_speed
        .export         _get_turbomaster_speed

        .include        "accelerator.inc"

_set_turbomaster_speed:
        tay
        lda     TURBOMASTER_SPEED_REG
        asl
        cpy     #SPEED_4X
        ror
store_speed:
        sta     TURBOMASTER_SPEED_REG

_get_turbomaster_speed:
        ldx     #$00
        lda     TURBOMASTER_SPEED_REG
        and     #$80
        beq     is_slow_speed
is_high_speed:
        lda     #SPEED_4X
is_slow_speed:
        rts

