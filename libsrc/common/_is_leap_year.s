;
; Colin Leroy-Mira, 2024
;
; unsigned char __fastcall__ IsLeapYear (unsigned char Year)
; Returns 1 in A if the given year is a leap year. Expects a year from 0 to 206,
; without 1900 added.
;

        .export         _IsLeapYear

_IsLeapYear:
        ldx     #$00            ; Prepare X for rts
        cmp     #$00            ; Y 0 (1900) is not a leap year
        beq     NotLeap
        cmp     #$C8            ; Y 200 (2100) is not a leap year
        beq     NotLeap
        and     #$03            ; Year % 4 == 0 means leap year
        bne     NotLeap
        lda     #$01            ; Return 1
        rts
NotLeap:
        lda     #$00            ; Return 0
        rts
