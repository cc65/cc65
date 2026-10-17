;
; long __fastcall__ f_lseek (long offset, int whence, int fildes);
;

        .export         _f_lseek

        .import         incsp6

        .importzp       c_sp, sreg

        .include        "rp6502.inc"

_f_lseek:
        sta     RIA_A           ; fildes
        ldy     #5
@push:  lda     (c_sp),y        ; offset, high byte first
        sta     RIA_XSTACK
        dey
        cpy     #1
        bne     @push
        lda     (c_sp)          ; whence
        sta     RIA_XSTACK
        lda     #RIA_OP_LSEEK
        sta     RIA_OP
        jsr     RIA_SPIN
        ldy     RIA_SREG
        sty     sreg
        ldy     RIA_SREG+1
        sty     sreg+1
        jmp     incsp6
