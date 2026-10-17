;
; int __fastcall__ read_xstack (void* buf, unsigned count, int fildes);
;

        .export         _read_xstack, __ria_read_xstack

        .import         popax, popptr1

        .importzp       ptr1, tmp1

        .include        "rp6502.inc"

_read_xstack:
        sta     RIA_A
        jsr     popax
        stx     RIA_XSTACK
        sta     RIA_XSTACK      ; count
        jsr     popptr1         ; buf, Y = 0
; read() enters here with fildes in RIA_A, the count pushed, buf in ptr1, Y = 0.
__ria_read_xstack = *
        lda     #RIA_OP_READ_XSTACK
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @done
        sta     tmp1
        pha
        phx
        txa
        beq     @rest
@page:  lda     RIA_XSTACK
        sta     (ptr1),y
        iny
        bne     @page
        inc     ptr1+1
        dex
        bne     @page
@rest:  cpy     tmp1
        beq     @end
        lda     RIA_XSTACK
        sta     (ptr1),y
        iny
        bra     @rest
@end:   plx
        pla
@done:  rts
