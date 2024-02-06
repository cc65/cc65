;
; Marco van den Heuvel, 2018-04-27
;

; unsigned char __fastcall__ set_c65_speed (unsigned char speed);
;
;/* Set the speed of the C65 CPU, using SPEED_SLOW will switch to
; * 1 Mhz mode, SPEED_3X or SPEED_FAST will switch to 3.5 Mhz (fast) mode.
; *
; * Note that any value higher or equal to SPEED_3X will switch to fast mode.
; *
; * This function will return the actual speed the CPU is at after trying
; * to set the requested speed, to my knowledge the switching should not fail.
; *
; * This function does not check for the presence of a C65/C64DX in C64 mode,
; * make sure you use 'detect_c65();' before using.
; */

; unsigned char get_c65_speed (void);
;
;/* Get the speed of the C65 CPU.
; *
; * Possible return values:
; * SPEED_SLOW  : 1 Mhz mode
; * SPEED_3X    : 3.5 Mhz mode
; *
; * This function does not check for the presence of a C65/C64DX in C64 mode,
; * make sure you use 'detect_c65();' before using.
; */

        .export         _set_c65_speed
        .export         _get_c65_speed

        .include        "accelerator.inc"

_set_c65_speed:
        tay
        jsr     activate_new_vic_mode
        cpy     #SPEED_3X
        bcs     high_speed
low_speed:
        and     #$BF
store_speed:
        sta     C65_VICIII_CTRL_B
        lda     C65_VICIII_CTRL_B
        jmp     return_c65_speed

high_speed:
        ora     #$40
        bne     store_speed

_get_c65_speed:
        jsr     activate_new_vic_mode
return_c65_speed:
        sta     C65_VICIII_KEY
        and     #$40
        beq     speed_is_slow              ; when this branch is taken then register A is already set to SPEED_SLOW
        lda     #SPEED_3X
speed_is_slow:
        ldx     #$00
        rts

activate_new_vic_mode:
        lda     #C65_VICIII_UNLOCK_1
        sta     C65_VICIII_KEY
        lda     #C65_VICIII_UNLOCK_2
        sta     C65_VICIII_KEY
        lda     C65_VICIII_CTRL_B
        rts
