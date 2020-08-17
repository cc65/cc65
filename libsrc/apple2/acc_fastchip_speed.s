;
; Marco van den Heuvel, 2018-05-04
;

; unsigned char __fastcall__ set_fastchip_speed (unsigned char speed);
;
;/* Set the speed of the Fast Chip slot card, the following inputs
; * are accepted: 
; * SPEED_SLOW : 1 Mhz mode
; * SPEED_1X   : 1 Mhz mode
; * SPEED_2X   : 2 Mhz mode
; * SPEED_3X   : 3.1 Mhz mode
; * SPEED_4X   : 4.1 Mhz mode
; * SPEED_5X   : 5 Mhz mode
; * SPEED_6X   : 6.2 Mhz mode
; * SPEED_7X   : 7.1 Mhz mode
; * SPEED_8X   : 8.3 Mhz mode
; * SPEED_10X  : 10 Mhz mode
; * SPEED_12X  : 12.5 Mhz mode
; * SPEED_16X  : 16.6 Mhz mode
; *
; * Note that any value higher or equal to SPEED_16X will switch to 16 Mhz mode.
; * SPEED_13X, SPEED_14X and SPEED15X will switch to 12 Mhz mode.
; * SPEED_11X will switch to 10 Mhz mode, and SPEED_9X will switch to 10 Mhz mode.
; *
; * This function will return the actual speed the CPU is at after trying
; * to set the requested speed, to my knowledge the switching should not fail.
; *
; * This function does not check for the presence of the Fast Chip slot card,
; * make sure you use 'detect_fastchip();' before using.
; */

; unsigned char get_fastchip_speed (void);
;
;/* Get the speed of the Fast Chip slot card.
; *
; * Possible return values:
; * SPEED_1X    : 1 Mhz mmode
; * SPEED_2X    : 2 Mhz mode
; * SPEED_3X    : 3.1 Mhz mode
; * SPEED_4X    : 4.1 Mhz mode
; * SPEED_5X    : 5 Mhz mode
; * SPEED_6X    : 6.2 Mhz mode
; * SPEED_7X    : 7.1 Mhz mode
; * SPEED_8X    : 8.3 Mhz mode
; * SPEED_10X   : 10 Mhz mode
; * SPEED_12X   : 12.5 Mhz mode
; * SPEED_16X   : 16.6 Mhz mode
; *
; * This function does not check for the presence of the Fast Chip slot card,
; * make sure you use 'detect_fastchip();' before using.
; */

        .export         _set_fastchip_speed
        .export         _get_fastchip_speed

        .include        "accelerator.inc"

_set_fastchip_speed:
        jsr     unlock_regs
        ldy     #10                      ; start at 16x speed
compare_set_speed_loop:
        cmp     accel_speed_table,y
        bcs     found_set_speed
        dey
        bne     compare_set_speed_loop
found_set_speed:
        lda     fastchip_speed_table,y
set_speed:
        sta     FASTCHIP_SPEED_REG       ; FIXME: use constant
        bne     return_speed

_get_fastchip_speed:
        jsr     unlock_regs
return_speed:
        lda     FASTCHIP_SPEED_REG       ; FIXME: use constant
        ldy     #FASTCHIP_LOCK
        sty     FASTCHIP_LOCK_REG        ; lock the registers
        ldy     #10                      ; start at 16x speed
compare_get_speed_loop:
        cmp     fastchip_speed_table,y
        bcs     found_get_speed
        dey
        bne     compare_get_speed_loop
found_get_speed:
        lda     accel_speed_table,y
        ldx     #$00
        rts

unlock_regs:
        ldy     #$04                ; loop 4 times
        lda     #FASTCHIP_UNLOCK    ; load the unlock value
unlock_loop:
        sta     FASTCHIP_LOCK_REG   ; store in lock register
        dey
        bne     unlock_loop
        sta     FASTCHIP_ENABLE_REG ; enable the Fast Chip
        rts


.rodata

; generic accelerator speeds for translation to/from Fast Chip speed values
accel_speed_table:
        .byte SPEED_1X
        .byte SPEED_2X
        .byte SPEED_3X
        .byte SPEED_4X
        .byte SPEED_5X
        .byte SPEED_6X
        .byte SPEED_7X
        .byte SPEED_8X
        .byte SPEED_10X
        .byte SPEED_12X
        .byte SPEED_16X

; Fast Chip speed values for translation to/from generic accelerator speeds
fastchip_speed_table:
        .byte FASTCHIP_SPEED_1X
        .byte FASTCHIP_SPEED_2X
        .byte FASTCHIP_SPEED_3X
        .byte FASTCHIP_SPEED_4X
        .byte FASTCHIP_SPEED_5X
        .byte FASTCHIP_SPEED_6X
        .byte FASTCHIP_SPEED_7X
        .byte FASTCHIP_SPEED_8X
        .byte FASTCHIP_SPEED_10X
        .byte FASTCHIP_SPEED_12X
        .byte FASTCHIP_SPEED_16X
