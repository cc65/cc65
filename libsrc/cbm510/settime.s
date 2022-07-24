;
; 2018-08-18, Oliver Schmidt
; 2018-08-19, Greg King
;
; int __fastcall__ clock_settime (clockid_t clk_id, const struct timespec *tp);
;

        .include        "time.inc"
        .include        "cbm510.inc"
        .include        "extzp.inc"

        .importzp       sreg, ptr1
        .import         pushax, pusheax, ldax0sp, ldeaxidx
        .import         sys_bank, restore_bank
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

        jsr     sys_bank
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
@L3:    ldy     #CIA::TODHR
        sta     (cia2),y
        lda     TM + tm::tm_min
        jsr     dec2BCD
        ldy     #CIA::TODMIN
        sta     (cia2),y
        lda     TM + tm::tm_sec
        jsr     dec2BCD
        ldy     #CIA::TODSEC
        sta     (cia2),y
        jsr     restore_bank

        jsr     ldax0sp
        ldy     #3+timespec::tv_nsec
        jsr     ldeaxidx
        jsr     pusheax
        jsr     load_tenth
        jsr     tosdiveax

        jsr     sys_bank
        ldy     #CIA::TOD10
        sta     (cia2),y
        jsr     restore_bank

        lda     #$00
        tax
        jmp     incsp3

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
