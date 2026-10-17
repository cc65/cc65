;
; int __fastcall__ read_xram (unsigned buf, unsigned count, int fildes);
;

        .export         _read_xram

        .import         incsp4

        .importzp       c_sp

        .include        "rp6502.inc"

_read_xram:
        sta     RIA_A           ; fildes
        ldy     #3
@push:  lda     (c_sp),y        ; buf, then count, high byte first
        sta     RIA_XSTACK
        dey
        bpl     @push
        lda     #RIA_OP_READ_XRAM
        sta     RIA_OP
        jsr     RIA_SPIN
        jmp     incsp4
