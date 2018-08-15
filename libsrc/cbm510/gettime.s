;
; Stefan Haubenthal, 2009-07-27
; Ullrich von Bassewitz, 2009-09-24
; Oliver Schmidt, 2018-08-14
;
; int clock_gettime (clockid_t clk_id, struct timespec *tp);
;

        .include        "time.inc"
        .include        "cbm510.inc"
        .include        "extzp.inc"

        .import         pushax, pusheax, tosmul0ax, steaxspidx, incsp1
        .import         sys_bank, restore_bank
        .importzp       sreg, tmp1, tmp2


;----------------------------------------------------------------------------
.code

.proc   _clock_gettime

        jsr     sys_bank
        jsr     pushax
        jsr     pushax

        ldy     #CIA::TODHR
        lda     (cia2),y
        bpl     AM
        and     #%01111111
        sed
        clc
        adc     #$12
        cld
AM:     jsr     BCD2dec
        sta     TM + tm::tm_hour
        ldy     #CIA::TODMIN
        lda     (cia2),y
        jsr     BCD2dec
        sta     TM + tm::tm_min
        ldy     #CIA::TODSEC
        lda     (cia2),y
        jsr     BCD2dec
        sta     TM + tm::tm_sec
        lda     #<TM
        ldx     #>TM
        jsr     _mktime

        ldy     #timespec::tv_sec
        jsr     steaxspidx      ; Pops address pushed by 2. pushax

        lda     #<(100 * 1000 * 1000 / $10000)
        ldx     #>(100 * 1000 * 1000 / $10000)
        sta     sreg
        stx     sreg+1
        lda     #<(100 * 1000 * 1000)
        ldx     #>(100 * 1000 * 1000)
        jsr     pusheax
        ldy     #CIA::TOD10
        lda     (cia2),y
        ldx     #>$0000
        jsr     tosmul0ax

        ldy     #timespec::tv_nsec
        jsr     steaxspidx      ; Pops address pushed by 1. pushax

        jsr     incsp1

        lda     #0
        tax
        jmp     restore_bank

.endproc

;----------------------------------------------------------------------------
; dec = (((BCD>>4)*10) + (BCD&0xf))

.proc   BCD2dec

        tax
        and     #%00001111
        sta     tmp1
        txa
        and     #%11110000      ; *16
        lsr                     ; *8
        sta     tmp2
        lsr
        lsr                     ; *2
        adc     tmp2            ; = *10
        adc     tmp1
        rts

.endproc

;----------------------------------------------------------------------------
; TM struct with date set to 1970-01-01
.data

TM:     .word           0       ; tm_sec
        .word           0       ; tm_min
        .word           0       ; tm_hour
        .word           1       ; tm_mday
        .word           0       ; tm_mon
        .word           70      ; tm_year
        .word           0       ; tm_wday
        .word           0       ; tm_yday
        .word           0       ; tm_isdst
