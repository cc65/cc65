;
; off_t __fastcall__ lseek (int fd, off_t offset, int whence);
;

        .export         _lseek

        .import         incsp6

        .importzp       c_sp, sreg

        .include        "rp6502.inc"

_lseek:
        pha                     ; whence
        ldy     #3
@push:  lda     (c_sp),y        ; offset, high byte first
        sta     RIA_XSTACK
        dey
        bpl     @push
        pla
        sta     RIA_XSTACK
        ldy     #4
        lda     (c_sp),y
        sta     RIA_A           ; fd
        lda     #RIA_OP_LSEEK
        sta     RIA_OP
        jsr     RIA_SPIN
        ldy     RIA_SREG
        sty     sreg
        ldy     RIA_SREG+1
        sty     sreg+1
        jmp     incsp6
