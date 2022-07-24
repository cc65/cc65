;
; Marco van den Heuvel, 2018-04-09
;

; unsigned char __fastcall__ set_scpu_speed (unsigned char speed);
;
;/* Set the speed of the SuperCPU cartridge, using SPEED_SLOW will switch to
; * 1 Mhz mode, SPEED_20X or SPEED_FAST will switch to 20 Mhz mode.
; *
; * Note that any value lower than SPEED_20X will switch to 1 Mhz mode, and
; * any value higher or equal to SPEED_20X will switch to 20 Mhz mode.
; *
; * This function will return the actual speed the CPU is at after trying
; * to set the requested speed, if this is not the speed that was requested
; * then possibly the hardware speed switch prevented any software speed
; * switching.
; *
; * This function does not check for the presence of the SuperCPU cartridge,
; * make sure you use 'detect_scpu();' before using.
; */

; unsigned char get_scpu_speed (void);
;
;/* Get the speed of the SuperCPU cartridge.
; *
; * Possible return values:
; * SPEED_1X    :  1 Mhz mode
; * SPEED_20X   : 20 Mhz mode
; *
; * This function does not check for the presence of the SuperCPU cartridge,
; * make sure you use 'detect_scpu();' before using.
; */

        .export         _set_scpu_speed
        .export         _get_scpu_speed

        .include        "accelerator.inc"

_set_scpu_speed:
        cmp     #SPEED_20X
        bcs     high_speed
low_speed:
        sta     SuperCPU_Slow
        jmp     _get_scpu_speed

high_speed:
        sta     SuperCPU_Fast

_get_scpu_speed:
        ldx     #$00
        lda     SuperCPU_Speed_Mode
        asl
        asl
        bcc     is_fast_speed
        lda     #SPEED_1X
        rts
is_fast_speed:
        lda     #SPEED_20X
        rts
