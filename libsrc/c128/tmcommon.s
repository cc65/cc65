;
; Oliver Schmidt, 16.8.2018
;
; Common stuff for the clock routines
;

        .include        "c128.inc"
        .include        "get_tv.inc"

        .export         TM, load_tenth

        .constructor    inittime
        .importzp       sreg
        .import         _get_tv


;----------------------------------------------------------------------------
.code

.proc   load_tenth

        lda     #<(100 * 1000 * 1000 / $10000)
        ldx     #>(100 * 1000 * 1000 / $10000)
        sta     sreg
        stx     sreg+1
        lda     #<(100 * 1000 * 1000)
        ldx     #>(100 * 1000 * 1000)
        rts

.endproc

;----------------------------------------------------------------------------
; Constructor that writes to the 1/10 sec register of the TOD to kick it
; into action. If this is not done, the clock hangs. We will read the register
; and write it again, ignoring a possible change in between.
.segment "ONCE"

.proc   inittime

        lda     CIA1_TOD10
        sta     CIA1_TOD10
        jsr     _get_tv
        cmp     #TV::PAL
        bne     @60Hz
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
