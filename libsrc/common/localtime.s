;
; Colin Leroy-Mira, 2024
;
; struct tm* __fastcall__ localtime (const time_t* timep);
;

        .export         _localtime
        .import         __time_t_to_tm, __tz
        .import         ldeaxi, tosaddeax, pusheax
        .importzp       sreg

_localtime:
        cpx     #$00            ; Check for null pointer
        bne     :+
        cmp     #$00
        beq     no_pointer
:       jsr     ldeaxi          ; Load value
        jsr     pusheax         ; Push it
        lda     __tz+1+3
        sta     sreg+1
        lda     __tz+1+2
        sta     sreg
        ldx     __tz+1+1
        lda     __tz+1
        jsr     tosaddeax       ; Add _tz.timezone
       jmp     __time_t_to_tm   ; Convert to struct tm

no_pointer:
        rts                     ; A/X already set
