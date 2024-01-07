;
; Colin Leroy-Mira, 2024
;
; struct tm* __fastcall__ gmtime (const time_t* timep);
;

        .export         _gmtime
        .import         __time_t_to_tm
        .import         ldeaxi

_gmtime:
        cpx     #$00            ; Check for null pointer
        bne     :+
        cmp     #$00
        beq     no_pointer
:       jsr     ldeaxi          ; Load value from pointer
        jmp     __time_t_to_tm  ; Convert it

no_pointer:
        rts                     ; A/X already set
