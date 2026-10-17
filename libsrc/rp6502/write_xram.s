;
; int __fastcall__ write_xram (unsigned buf, unsigned count, int fildes);
;

        .export         _write_xram

        .import         incsp4

        .importzp       c_sp

        .include        "rp6502.inc"

_write_xram:
        sta     RIA_A           ; fildes
        ldy     #3
@push:  lda     (c_sp),y        ; buf, then count, high byte first
        sta     RIA_XSTACK
        dey
        bpl     @push
        lda     #RIA_OP_WRITE_XRAM
        sta     RIA_OP
        jsr     RIA_SPIN
        jmp     incsp4
