;
; Colin Leroy-Mira, 2024
;
; struct tm* __fastcall__ _time_t_to_tm (const time_t t)
;
; Helper to gmtime and localtime. Breaks down a number of
; seconds since Jan 1, 1970 into days, hours and seconds,
; so that each of them fits in 16 bits; passes the
; result to _mktime which fixes all values in the struct,
; and returns a pointer to the struct to callers.
;

        .export         __time_t_to_tm
        .import         udiv32, _mktime
        .importzp       sreg, tmp3, ptr1, ptr2, ptr3, ptr4

        .include        "time.inc"

__time_t_to_tm:
        ; Divide number of seconds since epoch, in ptr1:sreg,
        ; by 86400 to get the number of days since epoch, and
        ; the number of seconds today in the remainder.

        ; Load t as dividend (sreg is already set by the caller)
        sta     ptr1
        stx     ptr1+1

        ; Load 86400 as divisor
        lda     #$80
        sta     ptr3
        lda     #$51
        sta     ptr3+1
        lda     #$01
        sta     ptr4
        lda     #$00
        sta     ptr4+1

        ; Clear TM buf while we have zero in A
        ldx     #.sizeof(tm)-1
:       sta     TM,x
        dex
        bpl     :-

        ; Divide t/86400
        jsr     udiv32

        ; Store the quotient (the number of full days), and increment
        ; by one as epoch starts at day 1.
        clc
        lda     ptr1
        adc     #1
        sta     TM + tm::tm_mday
        lda     ptr1+1
        adc     #0
        sta     TM + tm::tm_mday+1

        ; Now divide the number of remaining seconds by 3600,
        ; to get the number of hours, and the seconds in the
        ; current hour, in neat 16-bit integers.

        ; Load the previous division's remainder (in ptr2:tmp3:tmp4)
        ; as dividend
        lda     ptr2
        sta     ptr1
        lda     ptr2+1
        sta     ptr1+1
        lda     tmp3
        sta     sreg
        ; We ignore the high byte stored in tmp4 because it will be
        ; zero. We'll zero sreg+1 right below, when we'll have
        ; a convenient zero already in A.

        ; Load divisor
        lda     #<3600
        sta     ptr3
        lda     #>3600
        sta     ptr3+1

        ; Zero the two high bytes of the divisor and the high byte
        ; of the dividend.
        .if .cap(CPU_HAS_STZ)
        stz     ptr4
        stz     ptr4+1
        stz     sreg+1
        .else
        lda     #$00
        sta     ptr4
        sta     ptr4+1
        sta     sreg+1
        .endif

        ; Do the division
        jsr     udiv32

        ; Store year
        lda     #70
        sta     TM + tm::tm_year

        ; Store hours (the quotient of the last division)
        lda     ptr1
        sta     TM + tm::tm_hour
        lda     ptr1+1
        sta     TM + tm::tm_hour+1

        ; Store seconds (the remainder of the last division)
        lda     ptr2
        sta     TM + tm::tm_sec
        lda     ptr2+1
        sta     TM + tm::tm_sec+1

        ; The rest of the struct tm fields are zero. mktime
        ; will take care of shifting extra seconds to minutes,
        ; and extra days to months and years.

        ; Call mktime
        lda     #<TM
        ldx     #>TM
        jsr     _mktime

        ; And return our pointer
        lda     #<TM
        ldx     #>TM
        rts

        .bss

TM: .tag tm
