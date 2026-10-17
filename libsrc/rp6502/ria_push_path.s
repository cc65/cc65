;
; Push the zero-terminated path in A/X onto the XSTACK, last character first.
;
; On success N is clear, Y is 0 and ptr1 holds the path. A path longer than
; 255 characters pushes nothing, sets errno to EINVAL, and returns -1 in A/X
; with N set.
;

        .export         __ria_push_path

        .importzp       ptr1

        .include        "rp6502.inc"
        .include        "errno.inc"

__ria_push_path:
        sta     ptr1
        stx     ptr1+1
        ldy     #0
@len:   lda     (ptr1),y
        beq     @push
        iny
        bne     @len
        lda     #EINVAL
        jmp     ___directerrno
@push:  tya
        beq     @done
@next:  dey
        lda     (ptr1),y
        sta     RIA_XSTACK
        tya
        bne     @next
@done:  rts
