;
; Marco van den Heuvel, 2018-04-14
;

; unsigned char __fastcall__ set_c64dtv_speed (unsigned char speed);
;
;/* Set the speed of the C64DTV, using SPEED_SLOW will switch to
; * slow mode, SPEED_2X or SPEED_FAST will switch to fast mode.
; *
; * Note that any value higher or equal to SPEED_2X will switch to fast mode.
; *
; * This function will return the actual speed the CPU is at after trying
; * to set the requested speed, to my knowledge the switching should not fail.
; *
; * This function does not check for the presence of the C64DTV,
; * make sure you use 'detect_c64dtv();' before using.
; */

; unsigned char get_c64dtv_speed (void);
;
;/* Get the speed of the C64DTV.
; *
; * Possible return values:
; * SPEED_1X    : slow mode
; * SPEED_2X    : fast mode
; *
; * This function does not check for the presence of the C64DTV,
; * make sure you use 'detect_c64dtv();' before using.
; */

        .export         _set_c64dtv_speed
        .export         _get_c64dtv_speed

        .include        "accelerator.inc"

_set_c64dtv_speed:
        cmp     #SPEED_2X
        bcs     high_speed
low_speed:
        ldx     #C64DTV_Slow
set_speed:
        .byte   $32,$99               ; SAC #$99   set accumulator to reg 9 (cpu control)
        txa                           ; (re)set skip and burst bits
        .byte   $32,$00               ; SAC #$00   set accumulator back to reg 0
        jmp     _get_c64dtv_speed

high_speed:
        ldx     #C64DTV_Fast
        bne     set_speed


_get_c64dtv_speed:
        .byte   $32,$99               ; SAC #$99   set accumulator to reg 9 (cpu control)
        tax
        .byte   $32,$00               ; SAC #$00   set accumulator back to reg 0
        txa
        and     #C64DTV_Fast
        bne     in_fast_mode
        lda     #$00
        .byte   $2C
in_fast_mode:
        lda     #$01
        ldx     #$00
        rts
