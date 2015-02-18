;
; void clrscr (void);
;
        .export         _clrscr
        .import         plot
        .include        "extzp.inc"
        .include        "osic1p.inc"

; Adapted from the Challenger Character Graphics
; Reference Manual, "2.3.3 MACHINE LANGUAGE SCREEN CLEAR"
; This is self-modifying code!
BANKS = VIDEORAMSIZE / $100

_clrscr:
        lda       #$20          ; ' '
        ldy       #BANKS
        ldx       #$00
staloc:
        sta       SCRNBASE,X
        inx
        bne       staloc
        inc       staloc+2
        dey
        bne       staloc
        lda       #>(SCRNBASE)  ; Load high byte
        sta       staloc+2      ; Restore base address

        lda       #$00          ; Cursor in upper left corner
        sta       CURS_X
        sta       CURS_Y
        jmp       plot          ; Set the cursor position
