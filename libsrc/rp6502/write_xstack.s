;
; int __fastcall__ write_xstack (const void* buf, unsigned count, int fildes);
;

        .export         _write_xstack, __ria_write_xstack

        .import         popax, popptr1

        .importzp       ptr1

        .include        "rp6502.inc"
        .include        "errno.inc"

_write_xstack:
        sta     RIA_A
        jsr     popax
        pha
        phx
        jsr     popptr1
        plx
        ply                     ; count
        cpy     #1
        txa
        sbc     #>$200
        bcs     inval          ; count > $200
; write() enters here with fildes in RIA_A, buf in ptr1, the count in X:Y,
; and the carry clear.
__ria_write_xstack = *
        txa
        adc     ptr1+1
        sta     ptr1+1          ; buf + (count & $FF00)
        tya
        beq     @page
@push:  dey
        lda     (ptr1),y
        sta     RIA_XSTACK
        tya
        bne     @push
@page:  dec     ptr1+1
        dex
        bpl     @push
        lda     #RIA_OP_WRITE_XSTACK
        sta     RIA_OP
        jmp     RIA_SPIN
inval:  lda     #EINVAL
        jmp     ___directerrno
