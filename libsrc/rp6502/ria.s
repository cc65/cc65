;
; 2023, Rumbledethumps
;
; Helpers for building API shims

.include "rp6502.inc"

.export _ria_push_long, _ria_push_int
.export _ria_pop_long, _ria_pop_int
.export _ria_set_axsreg, _ria_set_ax
.export _ria_call_int, _ria_call_long

.importzp sreg

.code

; void __fastcall__ ria_push_long(unsigned long val);
_ria_push_long:
    ldy sreg+1
    sty RIA_XSTACK
    ldy sreg
    sty RIA_XSTACK
; void __fastcall__ ria_push_int(unsigned int val);
_ria_push_int:
    stx RIA_XSTACK
    sta RIA_XSTACK
    rts

; long __fastcall__ ria_pop_long(void);
_ria_pop_long:
    jsr _ria_pop_int
    ldy RIA_XSTACK
    sty sreg
    ldy RIA_XSTACK
    sty sreg+1
    rts

; int __fastcall__ ria_pop_int(void);
_ria_pop_int:
    lda RIA_XSTACK
    ldx RIA_XSTACK
    rts

; void __fastcall__ ria_set_axsreg(unsigned long axsreg);
_ria_set_axsreg:
    ldy sreg
    sty RIA_SREG
    ldy sreg+1
    sty RIA_SREG+1
; void __fastcall__ ria_set_ax(unsigned int ax);
_ria_set_ax:
    stx RIA_X
    sta RIA_A
    rts

; int __fastcall__ ria_call_int(unsigned char op);
_ria_call_int:
    sta RIA_OP
    jmp RIA_SPIN

; long __fastcall__ ria_call_long(unsigned char op);
_ria_call_long:
    sta RIA_OP
    jsr RIA_SPIN
    ldy RIA_SREG
    sty sreg
    ldy RIA_SREG+1
    sty sreg+1
    rts
