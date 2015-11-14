.include "gamate.inc"
	.zeropage
addr: .word 0
psa:	.word 0

.word 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0 ; get out of sensitiv area

readaddr: .word 0
editbuffer1: .byte 0,0,0,0, 0,0,0,0 ;0,1,2,3,4,5,6,7
writeaddr: .word 0
editbuffer2: .byte 0,0,0,0, 0,0,0,0 ;8,9,$a,$b,$c,$d,$e,$f
cursor: .byte 0
controlslast: .byte 0
controlsedge: .byte 0

	.data
temp_x:	.byte 0
temp_y:	.byte	0
temp_a:	.byte 0
irq_count: 	.byte 0
nmi_count:	.byte 0
psx:	.byte 0
psy:	.byte 0
xpos: .byte 0
ypos:	.byte 0

	.code
checksum:	.word 0
	.byte 1,0,1
	.byte "COPYRIGHT BIT CORPORATION", 0, $ff
	jmp	reset
	jmp nmi
	jmp irq

chars:
.incbin "cga2.chr"
			
hex2asc:	.byte "0123456789abcdef"

.proc nmi
			inc nmi_count
			rts
.endproc

.proc	irq
			inc irq_count
			rts
.endproc

.proc reset
			lda #>audio_base
			sta writeaddr+1
			sta readaddr+1
			lda #<audio_base
			sta writeaddr
			sta readaddr

			lda #$10
			sta editbuffer1+6
			lda #$e
			sta editbuffer2+5
			lda #$ff
			sta editbuffer2+3
			lda #$ff
			sta editbuffer2+4
			lda #$0f
			sta editbuffer2
			lda #$0f
			sta editbuffer2+1
			lda #$0e
			sta editbuffer2+2
			lda	#$38
			sta editbuffer1+7

			lda #0
			sta	lcd_x_pos
			sta lcd_y_pos
			sta irq_count
			sta cursor
			lda #1
			sta nmi_count
			cli
			lda	#lcd_mode_y_increment
			sta	lcd_mode

			jsr printy

			lda #1
			sta nmi_enable

loop:	
			lda irq_count
loop1:
			cmp irq_count
			beq	loop1
			lda irq_count
			and #7
			bne loop1		

			lda	#lcd_mode_y_increment
			sta	lcd_mode

			ldx #3
			ldy #32
			lda irq_count
			jsr printhex


			lda cursor
			ldy	#0
			cmp #20
			bcc firstline
			sec
			sbc	#20
			ldy #24
firstline:
			sta lcd_x
			sty lcd_y
			lda #' '
			jsr printsign
norclearcursor:
			
			jsr inputs

			lda irq_count
			and #8
			bne nocursor
			lda cursor
			ldy	#0
			cmp #20
			bcc firstline2
			sec
			sbc	#20
			ldy #24
firstline2:
			sta lcd_x
			sty lcd_y
			lda #'x'
			jsr printsign
nocursor:

			lda	#lcd_mode_y_increment
			sta	lcd_mode
			jsr printy			
			jmp loop
.endproc

.proc printy

			ldy #0
loop1: 
			tya
			pha
			asl
			tax
			lda readaddr,y
			ldy #8
			jsr printhex
			pla
			tay
			iny
			cpy #10
			bne loop1

loop2:
			tya
			pha
			tya
			sec
			sbc #10
			asl
			tax
			lda readaddr,y
			ldy #16
			jsr printhex
			pla
			tay
			iny
			cpy #20
			bne loop2

			ldx #0
			ldy #32
			lda nmi_count
			jsr printhex

			rts		
.endproc

.proc inputs
			lda controlslast
			eor controls
			and controlslast
			eor #$ff
			sta controlsedge
			and #up
			bne notup

			lda cursor
			lsr
			tay
			bcs uplow
			lda readaddr,y
			clc
			adc #$10
			sta readaddr,y
			jmp notup
uplow:lda readaddr,y
			clc
			adc #1
			sta readaddr,y
notup:lda controlsedge
			and #down
			bne notdown
			lda cursor
			lsr
			tay
			bcs downlow
			lda readaddr,y
			sec
			sbc #$10
			sta readaddr,y
			jmp notdown
downlow:
			lda readaddr,y
			sec
			sbc #1
			sta readaddr,y
notdown:lda controlsedge
			and #left
			bne notleft
			lda cursor
			beq notleft
			dec cursor
notleft:lda controlsedge
			and #right
			bne notright
			lda cursor
			cmp #40
			beq notright
			inc cursor
notright:lda controlsedge
			and #start
			bne notstart
			lda #0
			sta audio_base
			sta audio_base+1
			sta audio_base+2
			sta audio_base+3
			sta audio_base+4
			sta audio_base+5
			sta audio_base+6
			sta audio_base+8
			sta audio_base+9
			sta audio_base+10
			sta audio_base+11
			sta audio_base+12
			sta audio_base+13
			sta audio_base+7
notstart:lda controlsedge
			and #select
			bne notselect
			lda editbuffer1
			sta audio_base
			lda editbuffer1+1
			sta audio_base+1
			lda editbuffer1+2
			sta audio_base+2
			lda editbuffer1+3
			sta audio_base+3
			lda editbuffer1+4
			sta audio_base+4
			lda editbuffer1+5
			sta audio_base+5
			lda editbuffer1+6
			sta audio_base+6
			lda editbuffer2
			sta audio_base+8
			lda editbuffer2+1
			sta audio_base+9
			lda editbuffer2+2
			sta audio_base+10
			lda editbuffer2+3
			sta audio_base+11
			lda editbuffer2+4
			sta audio_base+12
			lda editbuffer2+5
			sta audio_base+13
			lda editbuffer1+7
			sta audio_base+7
notselect:lda controlsedge
			and #button_a
			bne notbuttona
			ldy #0
			ldy #0
writea:lda editbuffer1,y
			sta (writeaddr),y
			iny
			cpy	#8
			bne writea
writeb:lda editbuffer2-8,y
			sta (writeaddr),y
			iny
			cpy	#16
			bne writeb
notbuttona:lda controlsedge
			and #button_b
			bne notbuttonb
			ldy #0
reada:lda (readaddr),y
			sta editbuffer1,y
			iny
			cpy	#8
			bne reada
readb:lda (readaddr),y
			sta editbuffer2-8,y
			iny
			cpy	#16
			bne readb
notbuttonb:
			lda controls
			sta	controlslast
			rts
.endproc

.proc printstring
			sta	psa
			stx psa+1
			ldx #0
			stx	psx
			sty psy
printstring2:
			ldy	#0
			lda	(psa),y
			beq printstring1
			ldx psx
			stx	lcd_x
			ldy psy
			sty	lcd_y
			jsr printsign			
			inc psx
			lda psa
			clc
			adc	#1
			sta psa
			lda psa+1
			adc	#0
			sta psa+1
			jmp printstring2
printstring1:
			rts			
.endproc

.proc printstringy
			sta	psa
			stx psa+1
printstring2:
			ldy	#0
			lda	(psa),y
			beq printstring1
			jsr printsign			
			lda psa
			clc
			adc	#1
			sta psa
			lda psa+1
			adc	#0
			sta psa+1
			jmp printstring2
printstring1:
			rts			
.endproc


.proc printhex
			pha
			lsr
			lsr
			lsr
			lsr
			and #$0f
			stx	temp_x
			tax			
			lda	hex2asc,x
			ldx	temp_x
			stx	lcd_x
			sty	lcd_y
			jsr	printsign
			pla
			and	#$0f
			inx
			stx	temp_x
			tax
			lda	hex2asc,x
			ldx	temp_x
			stx	lcd_x
			sty	lcd_y
			jmp printsign
.endproc

.proc printsign
			sty	temp_y
			stx	temp_x
			sta	temp_a
			lda temp_a
			sta	addr
			lda #0
			sta addr+1
			asl	addr
			rol	addr+1
			asl	addr
			rol addr+1
			asl addr
			rol addr+1
			lda addr
			clc
			adc	#<chars
			sta	addr
			lda	addr+1
			adc	#>chars
			sta addr+1
			ldx	#8
			ldy #0
printsign1:
			lda	(addr),y
			sta	lcd_data
			iny
			dex
			bne printsign1
			ldx	temp_x
			ldy	temp_y
			rts
.endproc

