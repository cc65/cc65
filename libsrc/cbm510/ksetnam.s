;
; Ullrich von Bassewitz, 2003-12-18
;
; SETNAM kernal call.
;
; NOTE: The routine does not work like that in the CBM610 kernal (which works
; different than that on all other CBMs). Instead, it works like on all other
; Commodore machines. No segment has to be passed, the current segment is
; assumed.

        .export         SETNAM

        .import         sys_bank, restore_bank
        .import         sysp0: zp, ktmp: zp

        .include        "cbm510.inc"

.proc   SETNAM

        pha
        jsr     sys_bank
        sty     ktmp

        txa
        ldy     #$90                    ; FNAM
        sta     (sysp0),y

        lda     ktmp
        iny
        sta     (sysp0),y

        lda     ExecReg                 ; Assume name is always in this segment
        ldy     #$92                    ; FNAM_SEG
        sta     (sysp0),y

        ldy     #$9D                    ; FNAM_LEN
        pla
        sta     (sysp0),y
        ldy     ktmp
        jmp     restore_bank

.endproc



