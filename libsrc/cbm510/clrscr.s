;
; Ullrich von Bassewitz, 14.09.2001
;

        .export         _clrscr
        .import         plot

        .include        "cbm510.inc"
        .include        "extzp.inc"

; ------------------------------------------------------------------------
; void clrscr (void);

.proc   _clrscr

        lda     #0
        sta     CURS_X
        sta     CURS_Y
        jsr     plot            ; Set cursor to top left corner

        ldx     #$00            ; Count
        jsr     clearpage
        jsr     clearpage
        jsr     clearpage
        ldx     #<(40*25)       ; Count
        jsr     clearpage       ; Clear remainder of last page
        jmp     plot            ; Set screen pointer again

.endproc


.proc   clearpage

        txa
        pha                     ; Save count

        lda     #$20            ; Screencode for blank
        ldy     #$00
@L1:    sta     (SCREEN_PTR),y
        iny
        dex
        bne     @L1
        inc     SCREEN_PTR+1

        pla
        tax                     ; Reload count
        lda     IndReg
        pha
        lda     #$0F
        sta     IndReg          ; Switch to the system bank
        lda     CHARCOLOR       ; Load color
        ldy     #$00
@L2:    sta     (CRAM_PTR),y
        iny
        dex
        bne     @L2
        inc     CRAM_PTR+1
        pla
        sta     IndReg          ; Restore the old indirect bank
        rts

.endproc
