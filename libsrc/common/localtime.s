;
; Colin Leroy-Mira, 2024
;
; struct tm* __fastcall__ localtime (const time_t* timep);
;

        .export         _localtime
        .import         __time_t_to_tm, __tz, _tzset_time
        .import         ldeaxi, tosaddeax, pusheax
        .importzp       sreg, ptr1

_localtime:
        cpx     #$00            ; Check for null pointer
        bne     :+
        cmp     #$00
        beq     no_pointer
:       jsr     ldeaxi          ; Load value
        jsr     pusheax         ; Push it
        jsr     _tzset_time
        lda     __tz+1+3
        sta     sreg+1
        lda     __tz+1+2
        sta     sreg
        ldx     __tz+1+1
        lda     __tz+1
        jsr     tosaddeax       ; Add _tz.timezone
        jsr     __time_t_to_tm  ; Convert to struct tm
        sta     ptr1            ; Returned tm pointer
        stx     ptr1+1
        ldy     #16
        lda     __tz+0          ; Load _tz.daylight
        sta     (ptr1),y        ; Store to tm.tm_isdst
        lda     ptr1

no_pointer:
        rts                     ; A/X already set
