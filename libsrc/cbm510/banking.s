;
; Ullrich von Bassewitz, 28.09.1998
;
; Banking routines for the 610.
;

        .export         set_bank, sys_bank, restore_bank
        .importzp       ptr1, segsave

        .include        "cbm510.inc"

.code

.proc   sys_bank
        pha
        lda     IndReg
        sta     segsave
        lda     #$0F
        sta     IndReg
        pla
        rts
.endproc

.proc   set_bank
        pha
        lda     IndReg
        sta     segsave
        pla
        sta     IndReg
        rts
.endproc

.proc   restore_bank
        pha
        lda     segsave
        sta     IndReg
        pla
        rts
.endproc


