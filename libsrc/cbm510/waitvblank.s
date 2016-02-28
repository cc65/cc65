
        .export         _waitvblank
        .import         PALFLAG
        .import         sys_bank, restore_bank

        .importzp       vic

        .include        "cbm510/cbm510.inc"

_waitvblank:
        rts     ; FIXME

        jsr	sys_bank		; Switch to the system bank

        lda PALFLAG
        beq ntsc
        ldx #(312-24)-256
        .byte $2c
ntsc:
        ldx #(262-2)-256

        sei
        ldy #VIC_CTRL1
l1:
        lda (vic),y
        and #$80
        beq l1

;?!?
;		ldy #VIC_HLINE
;		txa
;l2:
;		cmp (vic),y
;		bcs l2

        cli
        jmp	restore_bank
