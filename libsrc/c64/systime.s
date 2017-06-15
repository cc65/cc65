;
; Stefan Haubenthal, 27.7.2009
;
; time_t _systime (void);
; /* Similar to time(), but:
; **   - Is not ISO C
; **   - Does not take the additional pointer
; **   - Does not set errno when returning -1
; */
;

        .include        "time.inc"
        .include        "c64.inc"
        .include        "get_tv.inc"

        .constructor    initsystime
        .importzp       tmp1, tmp2
        .import         _get_tv, _get_ostype


;----------------------------------------------------------------------------
.code

.proc   __systime

        lda     CIA1_TODHR
        bpl     AM
        and     #%01111111
        sed
        clc
        adc     #$12
        cld
AM:     jsr     BCD2dec
        sta     TM + tm::tm_hour
        lda     CIA1_TODMIN
        jsr     BCD2dec
        sta     TM + tm::tm_min
        lda     CIA1_TODSEC
        jsr     BCD2dec
        sta     TM + tm::tm_sec
        lda     CIA1_TOD10              ; Dummy read to unfreeze
        lda     #<TM
        ldx     #>TM
        jmp     _mktime

; dec = (((BCD>>4)*10) + (BCD&0xf))
BCD2dec:tax
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
; Constructor that writes to the 1/10 sec register of the TOD to kick it
; into action. If this is not done, the clock hangs. We will read the register
; and write it again, ignoring a possible change in between.
.segment "ONCE"

.proc   initsystime

        lda     CIA1_TOD10
        sta     CIA1_TOD10
        jsr     _get_tv
        cmp     #TV::PAL
        bne     @60Hz
        jsr     _get_ostype
        cmp     #$43
        beq     @60Hz
        lda     CIA1_CRA
        ora     #$80
        sta     CIA1_CRA
@60Hz:  rts

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
