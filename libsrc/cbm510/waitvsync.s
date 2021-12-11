;
; Written by Groepaz <groepaz@gmx.net>
;
; void waitvsync (void);
;

        .export         _waitvsync
        .import         PALFLAG
        .import         sys_bank, restore_bank

        .importzp       vic

        .include        "cbm510.inc"

_waitvsync:
        jsr     sys_bank        ; Switch to the system bank
        sei

        ldy     #VIC_CTRL1
@l1:
        lda     (vic),y
        bpl     @l1
@l2:
        lda     (vic),y
        bmi     @l2

        cli
        jmp    restore_bank
