;
; Written by Groepaz <groepaz@gmx.net>
;
; void waitvsync (void);
;

        .export         _waitvsync

        .include        "c64.inc"

_waitvsync:
@l1:
        bit     VIC_CTRL1
        bpl     @l1
@l2:
        bit     VIC_CTRL1
        bmi     @l2
        rts
