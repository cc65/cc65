;;https://www.lemon64.com/forum/viewtopic.php?t=33082
.segment "CODE"
.macpack cbm
scrcode
	;; top of border is line 50, bottom of visible screen is line 250
top = $32
bottom = $fa
start:
	;; lower case char mode
	lda #$06
	ora $d018
	sta $d018
	ldx #$00
showms:	
        lda message,x		;setup message
	;; start the horrible case conversion routine
	rol
	rol
	bcc ucase64
	rol
	bcc ucase32	;64 and 32 bit set
	ror			;sub 96 from chr
	ror
	ror
	sec
	sbc #$60
	jmp caseok
ucase32:
	ror
ucase64:
	ror
	ror
caseok:	; is it really all necessary!
	sta $0400,x
	inx
	cpx #$28
        bne showms
	
	sei
	lda #$7f		
        sta $dc0d		;block cia1 interrupts
	sta $dd0d		;block cia2 interrupts
	
        lda #$01
        sta $d01a		;enable raster interrupts

	jsr setupraster1
	cli
	rts
	
raster1:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lda #$00
	sta $d020		;do some flashy border stuff

	lda #$01
	sta $d019

	;; setup the next raster point
	sei
	jsr colourwash
	jsr setupraster2
	cli
	jmp $ea31		;call next interrupt

raster2:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lda #$02
	sta $d020		;do some flashy border stuff

	lda #$01
	sta $d019

	;; setup the next raster point
	sei
	jsr setupraster1
	cli
	jmp $ea31		;call next interrupt
	
setupraster1:
	lda #<raster1
        ldx #>raster1
        sta $0314		;change the interrupt vector
        stx $0315

	;; setup the raster point
	;; top
	lda #$1b
	sta $d011
	lda #top
        sta $d012		;interrupt at raster line 0
	rts
	
setupraster2:	
	lda #<raster2
        ldx #>raster2
        sta $0314		;change the interrupt vector
        stx $0315

	;; setup the raster point
	;; bottom
	lda #$1b
	sta $d011
	lda #bottom
        sta $d012		;interrupt at raster line 0
	rts

colourwash:
	lda colour+$00
        sta colour+$28
        ldx #$00
cycle:
	lda colour+$01,x
        sta colour+$00,x
        lda colour,x
        sta $d800,x
        inx
	cpx #$28
        bne cycle
        rts
	
colour:				;colour data tables
	.byte $09,$09,$02,$02,$08
        .byte $08,$0a,$0a,$0f,$0f
        .byte $07,$07,$01,$01,$01
        .byte $01,$01,$01,$01,$01
        .byte $01,$01,$01,$01,$01
        .byte $01,$01,$01,$07,$07
        .byte $0f,$0f,$0a,$0a,$08
        .byte $08,$02,$02,$09,$09
        .byte $00,$00,$00,$00,$00
	
message:			;data for message
	.byte "    The C64 was made "
	.byte "by Dr Rubbix !!!"
	.byte "                "
