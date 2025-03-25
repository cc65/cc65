;
; Colin Leroy-Mira, 2024
;
; time_t __fastcall__ mktime (register struct tm* TM)
;
; Converts a struct tm to a time_t timestamp, making sure
; day, month, year, hour, minute and seconds are in the
; correct range.
;

        .export         _mktime
        .import         __tz
        .import         pushax, pusha0, pusheax
        .import         shrax2, _div, tosumulax, tosumodax, tossubeax, tosaddeax, tosumuleax
        .importzp       ptr2, tmp3, sreg

        .include        "time.inc"

; ------------------------------------------------------------------------
; Special values

FEBRUARY   = 1
MARCH      = 2
JAN_1_1970 = 4
N_SEC      = 60
N_MIN      = 60
N_HOUR     = 24
N_MON      = 12
N_DAY_YEAR = 365
; ------------------------------------------------------------------------
; Helpers

        ; Helper to shift overflows from one field to the next
        ; Current field in Y, divisor in A
        ; Keeps remainder in current field, and adds the quotient
        ; to the next one
adjust_field:
        pha                     ; Push divisor
        iny                     ; Point to high byte of current field
        lda     (ptr2),y
        tax
        dey
        sty     tmp3            ; Store current field (_div will mess with
        lda     (ptr2),y        ; tmp1 and tmp2)
        jsr     pushax
        pla                     ; Load divisor
        ldx     #$00

        jsr     _div

        ldy     tmp3            ; Store remainder in current field
        sta     (ptr2),y
        iny
        txa
        sta     (ptr2),y

        lda     sreg            ; Add quotient to next field
        iny
        clc
        adc     (ptr2),y
        sta     (ptr2),y
        iny
        lda     sreg+1
        adc     (ptr2),y
        sta     (ptr2),y
        rts

        ; Returns 1 in A if the given year is a leap year. Expects a year
        ; from 0 to 206, without 1900 added.
is_leap_year:
        cmp     #$00            ; Y 0 (1900) is not a leap year
        beq     not_leap
        cmp     #$C8            ; Y 200 (2100) is not a leap year
        beq     not_leap
        and     #$03            ; Year % 4 == 0 means leap year
        bne     not_leap
        lda     #$01            ; Return 1
        rts
not_leap:
        lda     #$00            ; Return 0
        rts

        ; Returns the number of days in the current month/year in A
get_days_in_month:
        ldy     #tm::tm_mon
        lda     (ptr2),y
        tax
        lda     months_len,x
        cpx     #FEBRUARY
        beq     :+
        rts
:       tax
        ldy     #tm::tm_year    ; Adjust for leap years
        lda     (ptr2),y
        jsr     is_leap_year
        beq     :+
        inx
:       txa
        rts

        ; Add AX to counter
addaxcounter:
        clc
        adc     Counter
        sta     Counter            ; Store in Counter
        txa
        adc     Counter+1
        sta     Counter+1
        rts

        ; Helpers for long chain of arithmetic on day counter.
        ; Reload Counter and push it on the stack
load_and_push_counter:
        lda     Counter+3
        sta     sreg+1
        lda     Counter+2
        sta     sreg
        lda     Counter
        ldx     Counter+1
        jsr     pusheax
        rts

        ; Store result in AX:sreg to Counter
store_counter:
        sta     Counter
        stx     Counter+1
        lda     sreg
        sta     Counter+2
        lda     sreg+1
        sta     Counter+3
        rts

; ------------------------------------------------------------------------
; Code

_mktime:
        sta ptr2                ; Store struct to ptr2, which arithmetic
        stx ptr2+1              ; functions won't touch

        ; Check pointer validity
        ora ptr2+1
        bne :+
        lda     #$FF
        tax
        sta     sreg
        sta     sreg+1
        rts

        ; Adjust seconds
:       ldy     #tm::tm_sec
        lda     #N_SEC
        jsr     adjust_field

        ; Adjust minutes
        ldy     #tm::tm_min
        lda     #N_MIN
        jsr     adjust_field

        ; Adjust hours
        ldy     #tm::tm_hour
        lda     #N_HOUR
        jsr     adjust_field

        ;Shift one year as long as tm_mday is more than a year
        ldy     #tm::tm_year
        lda     (ptr2),y

dec_by_year:
        jsr     is_leap_year    ; Compute max numbers of days in year
        clc
        adc     #<N_DAY_YEAR    ; No care about carry,
        sta     Max             ; 365+1 doesn't overflow low byte

        ldy     #tm::tm_mday+1  ; Do we have more days in store?
        lda     (ptr2),y
        cmp     #>N_DAY_YEAR
        beq     :+              ; High byte equal, check low byte
        bcs     do_year_dec     ; High byte greater, decrement
        bcc     dec_by_month    ; Low byte lower, we're done
:       dey
        lda     (ptr2),y
        cmp     Max
        bcc     dec_by_month
        beq     dec_by_month

do_year_dec:
        ; Decrement days
        ldy     #tm::tm_mday
        lda     (ptr2),y
        sbc     Max             ; Carry already set
        sta     (ptr2),y
        iny
        lda     (ptr2),y
        sbc     #>N_DAY_YEAR
        sta     (ptr2),y

        ; Increment year
        ldy     #tm::tm_year
        lda     (ptr2),y
        clc
        adc     #1
        sta     (ptr2),y        ; No carry possible here either
        bcc     dec_by_year     ; bra, go check next year

dec_by_month:
        ; We're done decrementing days by full years, now do it
        ; month per month.
        ldy     #tm::tm_mon
        lda     #N_MON
        jsr     adjust_field

        ; Get max day for this month
        jsr     get_days_in_month
        sta     Max

        ; So, do we have more days than this month?
        ldy     #tm::tm_mday+1
        lda     (ptr2),y
        bne     do_month_dec    ; High byte not zero, sure we do
        dey
        lda     (ptr2),y
        cmp     Max
        bcc     calc_tm_yday    ; No
        beq     calc_tm_yday

do_month_dec:
        ; Decrement days
        ldy     #tm::tm_mday
        lda     (ptr2),y
        sec
        sbc     Max
        sta     (ptr2),y
        iny
        lda     (ptr2),y
        sbc     #$00
        sta     (ptr2),y

        ; Increment month
        ldy     #tm::tm_mon
        lda     (ptr2),y
        clc
        adc      #1
        sta      (ptr2),y

        bne     dec_by_month    ; Check next month

calc_tm_yday:
        ; We finished decrementing tm_mday and have put it in the correct
        ; year/month range. Now compute the day of the year.
        ldy     #tm::tm_mday    ; Get current day of month
        lda     (ptr2),y
        sta     Counter         ; Store it in Counter

        lda     #$00            ; Init counter high bytes
        sta     Counter+1
        sta     Counter+2
        sta     Counter+3

        ldy     #tm::tm_mon     ; Get current month
        lda     (ptr2),y
        asl
        tax
        clc
        lda     yday_by_month,x ; Get yday for this month's start
        adc     Counter         ; Add it to counter
        sta     Counter
        inx
        lda     yday_by_month,x
        adc     Counter+1
        sta     Counter+1

        ldy     #tm::tm_year    ; Adjust for leap years (if after feb)
        lda     (ptr2),y
        jsr     is_leap_year
        beq     dec_counter
        ldy     #tm::tm_mon     ; Leap year, get current month
        lda     (ptr2),y
        cmp     #MARCH
        bcs     store_yday

dec_counter:
        lda     Counter         ; Decrease counter by one (yday starts at 0),
        bne     :+              ; unless we're after february in a leap year
        dec     Counter+1
:       dec     Counter

store_yday:
        ldy     #tm::tm_yday    ; Store tm_yday
        lda     Counter
        sta     (ptr2),y
        iny
        lda     Counter+1
        sta     (ptr2),y

        ; Now calculate total day count since epoch with the formula:
        ; ((unsigned) (TM->tm_year-70)) * 365U +     (number of days per year since 1970)
        ;  (((unsigned) (TM->tm_year-(68+1))) / 4) + (one extra day per leap year since 1970)
        ;   TM->tm_yday                              (number of days in this year)

        ldy     #tm::tm_year    ; Get full years
        lda     (ptr2),y
        sec
        sbc     #70
        ldx     #0
        jsr     pushax
        lda     #<N_DAY_YEAR
        ldx     #>N_DAY_YEAR

        jsr     tosumulax
        jsr     addaxcounter

        ; Add one day per leap year
        ldy     #tm::tm_year    ; Get full years
        lda     (ptr2),y
        sec
        sbc     #69
        ldx     #0
        jsr     shrax2        ; Divide by 4

        jsr     addaxcounter

        ; Handle the 2100 exception (which was considered leap by "Add one day
        ; per leap year" just before)
        ldy     #tm::tm_year  ; Get full years
        lda     (ptr2),y
        cmp     #201
        bcc     finish_calc   ; <= 200, nothing to do

        lda     Counter
        bne     :+
        dec     Counter+1
:       dec     Counter

finish_calc:
        ; Now we can compute the weekday.
        lda     Counter
        clc
        adc     #JAN_1_1970
        pha
        lda     Counter+1
        adc     #0
        tax
        pla
        jsr     pushax

        lda     #7              ; Modulo 7
        ldx     #0
        jsr     tosumodax

        ldy     #tm::tm_wday    ; Store tm_wday
        sta     (ptr2),y
        iny
        txa
        sta     (ptr2),y

        ; DST
        lda     #$00            ; Store tm_isdst
        ldy     #tm::tm_isdst
        sta     (ptr2),y
        iny
        sta     (ptr2),y

        ; Our struct tm is all fixed and every field calculated.
        ; We can finally count seconds according to this formula:
        ; seconds = (full days since epoch) * 86400UL +
        ;            ((unsigned) TM->tm_hour) * 3600UL +
        ;            ((unsigned) TM->tm_min) * 60U +
        ;            ((unsigned) TM->tm_sec) -
        ;            _tz.timezone;

        ; We already have the number of days since epoch in our counter,
        ; from just before when we computed tm_wday. Reuse it.
        jsr     load_and_push_counter
        lda     #$00            ; Multiply by 86400
        sta     sreg+1
        lda     #$01
        sta     sreg
        lda     #$80
        ldx     #$51
        jsr     tosumuleax
        jsr     store_counter   ; Store into counter

        ; Push counter to add 3600 * hours to it
        jsr     load_and_push_counter

        ldx     #$00            ; Load hours
        stx     sreg
        stx     sreg+1
        ldy     #tm::tm_hour
        lda     (ptr2),y
        jsr     pusheax         ; Push
        ldx     #$00            ; Load 3600
        stx     sreg
        stx     sreg+1
        lda     #<3600
        ldx     #>3600
        jsr     tosumuleax      ; Multiply (pops the pushed hours)
        jsr     tosaddeax       ; Add to counter (pops the pushed counter)
        jsr     store_counter   ; Store counter

        ; Push counter to add 60 * min to it
        jsr     load_and_push_counter

        ldy     #tm::tm_min     ; Load minutes
        lda     (ptr2),y
        jsr     pusha0          ; Push
        lda     #N_MIN
        ldx     #0
        stx     sreg
        stx     sreg+1
        jsr     tosumulax       ; Multiply
        jsr     tosaddeax       ; Add to pushed counter
        jsr     store_counter   ; Store

        ; Add seconds
        jsr     load_and_push_counter

        ldy     #tm::tm_sec     ; Load seconds
        lda     (ptr2),y
        ldx     #0
        stx     sreg
        stx     sreg+1
        jsr     tosaddeax       ; Simple addition there

        ; No need to store/load/push the counter here, simply to push it
        ; for the last subtraction
        jsr     pusheax

        ; Subtract timezone
        lda     __tz+1+3
        sta     sreg+1
        lda     __tz+1+2
        sta     sreg
        ldx     __tz+1+1
        lda     __tz+1
        jsr     tossubeax

        ; And we're done!
        rts

        .data

months_len:
        .byte    31
        .byte    28
        .byte    31
        .byte    30
        .byte    31
        .byte    30
        .byte    31
        .byte    31
        .byte    30
        .byte    31
        .byte    30
        .byte    31

yday_by_month:
        .word    0
        .word    31
        .word    59
        .word    90
        .word    120
        .word    151
        .word    181
        .word    212
        .word    243
        .word    273
        .word    304
        .word    334


        .bss

Max:    .res 1                  ; We won't need a high byte
Counter:
        .res 4
