
        .export         _waitvsync

        .include        "plus4.inc"

_waitvsync:
@l1:
        lda     TED_VLINEHI
        and     #$01
        ora     TED_VLINELO
        bne     @l1
        rts
