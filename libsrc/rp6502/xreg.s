;
; 2023, Rumbledethumps
;
; CC65 will promote variadic char arguments to int. It will not demote longs.
; int __cdecl__ xreg(char device, char channel, unsigned char address, ...);

.export _xreg
.importzp sp
.import addysp, _ria_call_int_errno

.include "rp6502.inc"

.code

.proc _xreg

    ; save variadic size in X
    tya
    tax

@copy: ; copy stack
    dey
    lda (sp),y
    sta RIA_XSTACK
    tya
    bne @copy

    ; recover variadic size and move sp
    txa
    tay
    jsr addysp

    ; run RIA operation
    lda #RIA_OP_XREG
    jmp _ria_call_int_errno

.endproc
