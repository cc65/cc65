;
; Written by Groepaz <groepaz@gmx.net>
;
; void waitvsync (void);
;

        .export         _waitvsync

        .include        "vic20.inc"

_waitvsync:
@l2:
        lda     VIC_HLINE
        bne     @l2
        rts

