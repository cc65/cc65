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
@l3:
        lda     VDC_INDEX
        and     #$20
        beq     @l3
        rts
