;
; 2019-12-27, Greg King
;
; Common stuff for the clock routines
;

        .export         TM, load_jiffy

        .importzp       sreg


;----------------------------------------------------------------------------
; Load .EAX with the approximate number of nanoseconds
; in one jiffy (1/60th of a second).

.proc   load_jiffy

        lda     #<(17 * 1000 * 1000 / $10000)
        ldx     #>(17 * 1000 * 1000 / $10000)
        sta     sreg
        stx     sreg+1
        lda     #<(17 * 1000 * 1000)
        ldx     #>(17 * 1000 * 1000)
        rts

.endproc

;----------------------------------------------------------------------------
; TM struct with "is daylight-saving time" set to "unknown"
.data

TM:     .word   0               ; tm_sec
        .word   0               ; tm_min
        .word   0               ; tm_hour
        .word   0               ; tm_mday
        .word   0               ; tm_mon
        .word   0               ; tm_year
        .word   0               ; tm_wday
        .word   0               ; tm_yday
        .word   .loword(-1)     ; tm_isdst
