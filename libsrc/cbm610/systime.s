;
; Stefan Haubenthal, 2009-07-27
; Ullrich von Bassewitz, 2009-09-24
;
; time_t _systime (void);
; /* Similar to time(), but:
; **   - Is not ISO C
; **   - Does not take the additional pointer
; **   - Does not set errno when returning -1
; */
;

        .include        "time.inc"
        .include        "cbm610.inc"
        .include        "extzp.inc"

        .import         sys_bank, restore_bank
        .importzp       tmp1, tmp2


;----------------------------------------------------------------------------
.code

.proc   __systime

; Switch to the system bank

        jsr     sys_bank

; Read the clock

        ldy     #CIA::TODHR
        lda     (cia),y
        bpl     AM
        and     #%01111111
        sed
        clc
        adc     #$12
        cld
AM:     jsr     BCD2dec
        sta     TM + tm::tm_hour
        ldy     #CIA::TODMIN
        lda     (cia),y
        jsr     BCD2dec
        sta     TM + tm::tm_min
        ldy     #CIA::TODSEC
        lda     (cia),y
        jsr     BCD2dec
        sta     TM + tm::tm_sec
        ldy     #CIA::TOD10
        lda     (cia),y                 ; Dummy read to unfreeze

; Restore the bank

        jsr     restore_bank

; Convert to a time

        lda     #<TM
        ldx     #>TM
        jmp     _mktime

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

