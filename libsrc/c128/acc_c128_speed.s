;
; 2018-04-23, Marco van den Heuvel
; 2018-04-26, Greg King
;

; unsigned char __fastcall__ set_c128_speed (unsigned char speed);
;
;/* Set the speed of the C128 8502 CPU; using SPEED_SLOW will switch to
; * 1 Mhz (slow) mode, SPEED_2X or SPEED_FAST will switch to 2Mhz (fast) mode.
; *
; * Note that any value higher or equal to SPEED_2X will switch to fast mode.
; *
; * This function will return the actual speed the CPU is at, after trying
; * to set the requested speed; to my knowledge, the switching should not fail.
; *
; * For C64 programs, a check for a C128 in C64 mode is needed; make sure you
; * use 'detect_c128();' before using.
; *
; * For C128 programs, no detect function call is needed.
; */

; unsigned char get_c128_speed (void);
;
;/* Get the speed of the C128 8502 CPU.
; *
; * Possible return values:
; * SPEED_SLOW  : Slow mode
; * SPEED_2X    : Fast mode
; *
; * For C64 programs, a check for a C128 in C64 mode is needed; make sure you
; * use 'detect_c128();' before using.
; *
; * For C128 programs, no detect function call is needed.
; */

        .export         _set_c128_speed
        .export         _get_c128_speed

        .include        "accelerator.inc"

_set_c128_speed:
        cmp     #SPEED_2X
        lda     #$00                    ; clear VIC-IIe test bit
        rol     a                       ; carry flag is speed bit
        sta     C128_VICIIE_CLK

_get_c128_speed:
        lda     C128_VICIIE_CLK
        and     #$01
        ldx     #>$0000
        rts
