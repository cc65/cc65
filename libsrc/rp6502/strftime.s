;
; size_t __fastcall__ strftime (char* buf, size_t bufsize, const char* format,
;                               const struct tm* tm);
;

        .export         _strftime

        .import         incsp4, popptr1
        .import         ___seterrno

        .importzp       c_sp, ptr1, ptr2

        .include        "rp6502.inc"
        .include        "errno.inc"

_strftime:
        sta     ptr2            ; tm
        stx     ptr2+1
        jsr     popptr1         ; format, Y = 0
@end:   lda     (ptr1),y
        beq     @tm
        iny
        bne     @end
        lda     #EINVAL         ; strings are capped at 255 bytes
        jsr     ___seterrno
        bra     @fail
@tm:    phy                     ; format length
        ldy     #17             ; sizeof(struct tm)-1, pushed in reverse
@tmb:   lda     (ptr2),y
        sta     RIA_XSTACK
        dey
        bpl     @tmb
        stz     RIA_XSTACK      ; format terminator
        ply
@fmt:   tya                     ; format is pushed from its end
        beq     @call
        dey
        lda     (ptr1),y
        sta     RIA_XSTACK
        bra     @fmt
@call:  lda     #RIA_OP_STRFTIME
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @fail           ; errno set by OS
        sta     ptr2
        stx     ptr2+1
        cmp     (c_sp)          ; the result and its terminator must fit
        txa
        ldy     #1
        sbc     (c_sp),y
        bcs     @drop
        iny
        lda     (c_sp),y
        sta     ptr1
        iny
        lda     (c_sp),y
        sta     ptr1+1
        ldx     ptr2+1
        ldy     ptr2
@pop:   tya
        bne     @char
        txa
        beq     @done
        dex
@char:  dey
        lda     RIA_XSTACK
        sta     (ptr1)
        inc     ptr1
        bne     @pop
        inc     ptr1+1
        bra     @pop
@done:  sta     (ptr1)          ; A = 0 terminates buf
        lda     ptr2
        ldx     ptr2+1
        jmp     incsp4
@drop:  stz     RIA_OP          ; RIA_OP_DROP_XSTACK
@fail:  lda     #0
        tax
        jmp     incsp4
