;
; void clrscr (void);
;
        .export         _clrscr
        .include        "c1p.inc"

; Adapted from the Challenger Character Graphics
; Reference Manual, "2.3.3 MACHINE LANGUAGE SCREEN CLEAR"
; This is self-modifying code!
BANKS = VIDEORAMSIZE / $100

_clrscr:	LDA #$20 ;' '
		LDY #BANKS
		LDX #$00
STALOC:		STA SCRNBASE,X
		INX
		BNE STALOC
		INC STALOC+2
		DEY
		BNE STALOC
		LDA #>(SCRNBASE) ; load high byte
		STA STALOC+2     ; restore base address
		RTS
