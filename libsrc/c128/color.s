;
; Ullrich von Bassewitz, 06.08.1998
;
; unsigned char __fastcall__ textcolor (unsigned char color);
; unsigned char __fastcall__ bgcolor (unsigned char color);
; unsigned char __fastcall__ bordercolor (unsigned char color);
;

 	.export		_textcolor, _bgcolor, _bordercolor

	.include	"c128.inc"


.proc   _textcolor

	bit	MODE		; Check 80/40 column mode
       	bmi     @L1		; Jump if 40 columns
        ldx	CHARCOLOR	; get old value
 	sta	CHARCOLOR	; set new value
	txa
        ldx     #$00
	rts

@L1:    tax                     ; Move new color to X
        lda     CHARCOLOR       ; Get old color + attributes
        and     #$F0            ; Keep old attributes
	ora     $CE5C,x		; Translate VIC color -> VDC color
        ldx     CHARCOLOR       ; Get the old color
        sta     CHARCOLOR       ; Set the new color + old attributes
        txa                     ; Old color -> A
        and     #$0F            ; Mask out attributes
        ldx     #$00            ; Load high byte
        rts

.endproc


.proc   _bgcolor

	ldx	VIC_BG_COLOR0	; get old value
	sta	VIC_BG_COLOR0	; set new value
	txa
        ldx     #$00
	rts

.endproc


.proc   _bordercolor

       	ldx    	VIC_BORDERCOLOR	; get old value
	sta	VIC_BORDERCOLOR	; set new value
	txa
        ldx     #$00
	rts

.endproc

