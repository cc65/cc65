;
; RIA time_t result handling shared by time() and mktime().
;

        .export         __ria_call_time

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
        sta     RIA_OP
        jsr     RIA_SPIN             ; N = sign of int result
        bmi     @fail                ; negative = OS error, errno set by OS
        lda     RIA_XSTACK           ; A:X:sreg = low 32 bits
        ldx     RIA_XSTACK
        ldy     RIA_XSTACK
        sty     sreg
        ldy     RIA_XSTACK
        sty     sreg+1
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
@fail:  lda     #$FF                 ; (time_t)-1
        tax
        sta     sreg
        sta     sreg+1
        sta     tmp1
        sec                          ; failure
        rts
