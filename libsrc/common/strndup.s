;
; Colin Leroy-Mira, 03.07.2025
;
; char* __fastcall__ strndup (const char* S, size_t maxlen);
;

        .importzp       ptr4, c_sp
        .import         _strdup, _strlen, pushax, popax, _realloc

        .export         _strndup

_strndup:
        sta     maxlen        ; Remember maxlen
        stx     maxlen+1

        jsr     popax         ; Duplicate string
        jsr     _strdup
        jsr     pushax        ; Remember result

        jsr     _strlen       ; Check length,
        cpx     maxlen+1
        bcc     out           ; Return directly if < maxlen
        bne     :+
        cmp     maxlen
        bcc     out

:       ldy     #$00          ; Otherwise, point to end of string,
        lda     maxlen
        clc
        adc     (c_sp),y
        sta     ptr4
        lda     maxlen+1
        iny
        adc     (c_sp),y
        sta     ptr4+1

        dey                   ; Cut it short,
        tya
        sta     (ptr4),y

        ldx     maxlen+1      ; And finally, realloc to maxlen+1
        ldy     maxlen
        iny
        tya
        bne     :+
        inx
:       jsr     _realloc      ; TOS still contains result
                              ; We consider realloc will not fail,
                              ; as the block shrinks.
        jsr     pushax        ; push/pop for size optimisation
out:
        jmp     popax

        .bss

maxlen: .res 2
