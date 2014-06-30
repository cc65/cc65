;
; Oliver Schmidt, 22.08.2006
;
; time_t _systime (void);
; /* Similar to time(), but:
; **   - Is not ISO C
; **   - Does not take the additional pointer
; **   - Does not set errno when returning -1
; */
;

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "mli.inc"

__systime:
        ; Update time
        lda     #GET_TIME_CALL
        ldx     #GET_TIME_COUNT
        jsr     callmli
        bcs     err

        lda     DATELO+1
        lsr
        php                     ; Save month msb
        cmp     #70             ; Year < 70?
        bcs     :+              ; No, leave alone
        adc     #100            ; Move 19xx to 20xx
:       sta     TM + tm::tm_year
        lda     DATELO
        tax                     ; Save day
        plp                     ; Restore month msb
        ror
        lsr
        lsr
        lsr
        lsr
        beq     err             ; [1..12] allows for validity check
        tay
        dey                     ; Move [1..12] to [0..11]
        sty     TM + tm::tm_mon
        txa                     ; Restore day
        and     #%00011111
        sta     TM + tm::tm_mday

        lda     TIMELO+1
        sta     TM + tm::tm_hour
        lda     TIMELO
        sta     TM + tm::tm_min

        lda     #<TM
        ldx     #>TM
        jmp     _mktime

err:    lda     #$FF
        tax
        sta     sreg
        sta     sreg+1
        rts                     ; Return -1

        .bss

TM:     .tag    tm
