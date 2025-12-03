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

_tzset_time:
        rts
