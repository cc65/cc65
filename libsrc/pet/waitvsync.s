;
; Written by Robin Harbron, requires 12" monitor
;
; void waitvsync (void);
;

        .export         _waitvsync

        .include        "pet.inc"

_waitvsync:
        lda     #%00100000
:       and     VIA_PB
        bne     :-
        rts
