;
; Ullrich von Bassewitz, 2003-12-19
;
; READST kernal call
;

        .export         READST

        .import         sys_bank, restore_bank
        .import         sysp0: zp, ktmp: zp

        .include        "cbm610.inc"


.proc   READST

        jsr     sys_bank
        sty     ktmp                    ; Save Y register
        ldy     #ST
        lda     (sysp0),y               ; Load ST from system bank
        ldy     ktmp
        jmp     restore_bank            ; Will set condition codes on A

.endproc


