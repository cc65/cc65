;
; Oliver Schmidt, 16.8.2018
;
; int __fastcall__ clock_settime (clockid_t clk_id, const struct timespec *tp);
;

        .include        "time.inc"
        .include        "c128.inc"

        .importzp       sreg, ptr1
        .import         pushax, pusheax, ldax0sp, ldeaxidx
        .import         tosdiveax, incsp3, return0
        .import         TM, load_tenth


;----------------------------------------------------------------------------
.code

.proc   _clock_settime

        jsr     pushax

        .assert timespec::tv_sec = 0, error
        jsr     _localtime
        sta     ptr1
        stx     ptr1+1
        ldy     #.sizeof(tm)-1
@L1:    lda     (ptr1),y
        sta     TM,y
        dey
        bpl     @L1

        lda     TM + tm::tm_hour
        jsr     dec2BCD
        tax                     ; Force flags
        bne     @L2
        lda     #$92            ; 12 AM
        bne     @L3
@L2:    cmp     #$13            ; 1 PM
        bcc     @L3
        sed
        sbc     #$12
        cld
        ora     #%10000000
@L3:    sta     CIA1_TODHR
        lda     TM + tm::tm_min
        jsr     dec2BCD
        sta     CIA1_TODMIN
        lda     TM + tm::tm_sec
        jsr     dec2BCD
        sta     CIA1_TODSEC

        jsr     ldax0sp
        ldy     #3+timespec::tv_nsec
        jsr     ldeaxidx
        jsr     pusheax
        jsr     load_tenth
        jsr     tosdiveax
        sta     CIA1_TOD10

        jsr     incsp3
        jmp     return0

.endproc

;----------------------------------------------------------------------------
; Just sum up the value in BCD mode.
; http://forum.6502.org/viewtopic.php?p=7629#p7629

.proc   dec2BCD

        tax
        dex
        bmi     @L9
        lda     #0
        clc
        sed
@L1:    adc     #1
        dex
        bpl     @L1
        cld
@L9:    rts

.endproc
