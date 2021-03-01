;
; Written by Robin Harbron, requires 12" monitor
;
; void waitvsync (void);
;

        .export         _waitvsync

        .include        "pet.inc"

_waitvsync:
@l1:
        lda     VIA_PB
        and     #%00100000
        bne     @l1
        rts
