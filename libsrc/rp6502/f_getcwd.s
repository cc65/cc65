;
; int __fastcall__ f_getcwd (char* name, int size);
;

        .export         _f_getcwd, __ria_getcwd

        .import         popptr1

        .importzp       ptr1, ptr2, tmp1

        .include        "rp6502.inc"
        .include        "errno.inc"

_f_getcwd:
        ldy     #ENOMEM
; Y is the errno for a name that does not fit, which is ERANGE for getcwd().
__ria_getcwd:
        sty     tmp1
        sta     ptr2
        stx     ptr2+1          ; size
        jsr     popptr1         ; name, Y = 0
        lda     #RIA_OP_GETCWD
        sta     RIA_OP
        jsr     RIA_SPIN        ; length including the terminator
        bmi     @done
        pha
        clc
        sbc     ptr2
        txa
        sbc     ptr2+1
        bcs     @long           ; length > size
@copy:  lda     RIA_XSTACK
        sta     (ptr1),y
        beq     @end
        iny
        bne     @copy
        inc     ptr1+1
        bra     @copy
@end:   pla
@done:  rts
@long:  pla
        stz     RIA_OP          ; RIA_OP_DROP_XSTACK
        lda     tmp1
        jmp     ___directerrno
