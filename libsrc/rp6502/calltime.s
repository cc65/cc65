;
; RIA time_t result handling shared by time() and mktime().
;

        .export         __ria_call_time, __ria_time_fail

        .import         _ria_call_int, _ria_pop_long
        .import         ___seterrno

        .importzp       sreg, tmp1

        .include        "rp6502.inc"
        .include        "errno.inc"

;--------------------------------------------------------------------------
; Run RIA op A; it returns a 64-bit time_t on the xstack.
; Success:  A:X:sreg = time_t (low 32 bits), tmp1 = low byte, carry CLEAR.
; Failure:  A:X:sreg = (time_t)-1, tmp1 = $FF, carry SET.
;           errno = ERANGE on overflow; OS sets errno on its own errors.

__ria_call_time:
        jsr     _ria_call_int        ; N = sign of int result (see ria_call_int)
        bmi     __ria_time_fail      ; negative = OS error, errno set by OS
        jsr     _ria_pop_long        ; A:X:sreg = low 32 bits
        sta     tmp1
        lda     RIA_XSTACK           ; high 32 bits must be zero for
        ora     RIA_XSTACK           ; a 32-bit time_t
        ora     RIA_XSTACK
        ora     RIA_XSTACK
        bne     @overflow
        lda     tmp1
        clc                          ; success
        rts
@overflow:
        lda     #ERANGE
        jsr     ___seterrno
__ria_time_fail:                     ; public entry: just the -1 sentinel
        lda     #$FF                 ; (time_t)-1
        tax
        sta     sreg
        sta     sreg+1
        sta     tmp1
        sec                          ; failure
        rts
