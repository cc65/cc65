; tzset_time.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; void __fastcall__ tzset_time (time_t* time);
;

        .export         _tzset_time
        .import         _clock_gettimezone, __tz
        .import         pushax, ldax0sp, ldeaxi, pusheax, pusha, incsp2
        .include        "time.inc"


_tzset_time:
        jsr     ldeaxi
        jsr     pusheax
        lda     #CLOCK_REALTIME
        jsr     pusha
        lda     #<(__tz)
        ldx     #>(__tz)
        jmp     _clock_gettimezone
