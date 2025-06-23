;
; Written by Groepaz <groepaz@gmx.net>
;
; void waitvsync (void);
;

        .export         _waitvsync

        .include        "c128.inc"

_waitvsync:

        bit     MODE
        bmi     @c80

@l1:
        bit     VIC_CTRL1
        bpl     @l1
@l2:
        bit     VIC_CTRL1
        bmi     @l2
        rts

@c80:
        ;FIXME: do we have to switch banks?
        lda     #$20
@l3:
        and     VDC_INDEX
        beq     @l3
        rts
