;
; Marco van den Heuvel, 2018-04-20
;

; extern unsigned char __fastcall__ set_c128_speed (unsigned char speed);
;
;/* Set the speed of the C128 CPU, using SPEED_SLOW will switch to
; * 1 Mhz (slow) mode, SPEED_2X or SPEED_FAST will switch to 2Mhz (fast) mode.
; *
; * Note that any value higher or equal to SPEED_2X will switch to fast mode.
; *
; * This function will return the actual speed the CPU is at after trying
; * to set the requested speed, to my knowledge the switching should not fail.
; *
; * This function does not check for the presence of a C128 in C64 mode,
; * make sure you use 'detect_c128();' before using.
; */

; extern unsigned char get_c128_speed (void);
;
;/* Get the speed of the C128 CPU.
; *
; * Possible return values:
; * SPEED_SLOW  : Slow mode
; * SPEED_2X    : Fast mode
; *
; * This function does not check for the presence of a C128 in C64 mode,
; * make sure you use 'detect_c128();' before using.
; */

        .export         _set_c128_speed
        .export         _get_c128_speed

        .include        "accelerator.inc"

_set_c128_speed:
        cmp     #SPEED_2X
        bcs     high_speed
store_speed:
        sta     C128_VICIIE_CLK
        jmp     _get_c128_speed

high_speed:
        lda     #$01
        bne     store_speed


_get_c128_speed:
        lda     C128_VICIIE_CLK
        and     #$01
        ldx     #$00
        rts

