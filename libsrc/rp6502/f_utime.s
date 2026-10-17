;
; int __fastcall__ f_utime (const char* path, unsigned fdate, unsigned ftime,
;                           unsigned crdate, unsigned crtime);
;

        .export         _f_utime

        .import         __ria_push_path
        .import         incsp8

        .importzp       c_sp

        .include        "rp6502.inc"

_f_utime:
        sta     RIA_A
        stx     RIA_X           ; crtime
        ldy     #7
        lda     (c_sp),y
        tax
        dey
        lda     (c_sp),y
        jsr     __ria_push_path
        bmi     @done
        ldy     #5
@date:  lda     (c_sp),y        ; fdate, ftime, crdate, high byte first
        sta     RIA_XSTACK
        dey
        bpl     @date
        lda     #RIA_OP_UTIME
        sta     RIA_OP
        jsr     RIA_SPIN
@done:  jmp     incsp8
