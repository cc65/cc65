;
; RIA struct tm transfer for gmtime and localtime.
;

        .export         __rp6502_tm_call

        .import         _ria_call_int

        .include        "rp6502.inc"

;--------------------------------------------------------------------------
; Run a RIA op that returns a struct tm on the xstack. Op in A.
; Returns A/X = &__rp6502_tm, or NULL on error.

__rp6502_tm_call:
        jsr     _ria_call_int   ; A/X = result int, N = its sign
        bmi     @fail           ; negative = error, errno set by OS
        ldy     #0
@loop:  lda     RIA_XSTACK
        sta     __rp6502_tm,y
        iny
        cpy     #18             ; sizeof(struct tm)
        bne     @loop
        lda     #<__rp6502_tm
        ldx     #>__rp6502_tm
        rts
@fail:  lda     #0
        tax
        rts

;--------------------------------------------------------------------------
        .bss

__rp6502_tm:    .res 18
