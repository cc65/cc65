;
; Ullrich von Bassewitz, 2002-12-03
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;
; Changed to actually detect the mode instead of using a flag
; Marco van den Heuvel, 2018-03-08
;
; The detection goes wrong on accelerated machines for now.
;

        .include        "c64.inc"
        .include        "get_tv.inc"

;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        php
        sei
timing_loop_0:
        lda     VIC_HLINE
timing_loop_1:
        cmp     VIC_HLINE
        beq     timing_loop_1
        bmi     timing_loop_0
        and     #$03
        cmp     #$01
        bne     check_ntsc
        lda     #TV::NTSC_OLD         ; NTSC OLD constant
        bne     detected
check_ntsc:
        cmp     #$03
        bcc     ntsc

; check for PAL and PAL-N

        ldx     #$00
        lda     #$10
timing_loop_2:
        inx
        cmp     VIC_HLINE
        bne     timing_loop_2
        lda     #TV::PAL              ; PAL constant
        cpx     #$70
        bcc     detected
        lda     #TV::PAL_N            ; PAL-N constant
detected:
        ldx     #$00
        plp
        rts

ntsc:
        lda     #TV::NTSC             ; NTSC constant
        beq     detected

.endproc
