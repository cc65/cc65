;
; Ullrich von Bassewitz, 22.09.1998
;
; void clrscr (void);
;

        .export         _clrscr
        .import         plot
        .import         CURS_X: zp, CURS_Y: zp, CharPtr: zp

        .include        "cbm610.inc"

.proc   _clrscr

        lda     #0
        sta     CURS_X
        sta     CURS_Y
        jsr     plot            ; Set cursor to top left corner

        lda     IndReg
        pha
        lda     #$0F
        sta     IndReg          ; Switch to the system bank

        ldx     #8
        ldy     #$00
        lda     #$20            ; Screencode for blank
L1:     sta     (CharPtr),y
        iny
        bne     L1
        inc     CharPtr+1
        dex
        bne     L1

        pla
        sta     IndReg          ; Restore old indirect segment

        jmp     plot            ; Set screen pointer again

.endproc



