;
; time_t __fastcall__ time (time_t* timep);
;

        .export         _time

        .import         __ria_call_time

        .importzp       ptr1, sreg, tmp1

        .include        "rp6502.inc"

_time:
        sta     ptr1
        stx     ptr1+1          ; Save timep
        lda     #RIA_OP_TIME_GET
        jsr     __ria_call_time      ; A:X:sreg = time_t (or -1), tmp1 = low byte
        lda     ptr1
        ora     ptr1+1          ; timep == NULL?
        beq     @done
        ldy     #0
        lda     tmp1
        sta     (ptr1),y
        iny
        txa
        sta     (ptr1),y
        iny
        lda     sreg
        sta     (ptr1),y
        iny
        lda     sreg+1
        sta     (ptr1),y
@done:  lda     tmp1
        rts
