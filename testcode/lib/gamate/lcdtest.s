
        .export reset, irq, nmi ; FIXME


.include "gamate.inc"

	.zeropage
addr: .word 0
psa:	.word 0

	.data
temp_x:	.byte 0
temp_y:	.byte	0
temp_a:	.byte 0
irq_count: 	.byte 0
nmi_count:	.byte 0
psx:	.byte 0
psy:	.byte 0
count: .word 0
counted: .word 0
xpos: .byte 0
ypos:	.byte 0

	.code

chars:
.incbin "cga2.chr"
			
hex2asc:	.byte "0123456789abcdef"
format:	.byte "IrqNmiCountXposYpos", 0
xdesc:	.byte "0123456789abcdefghijklmnopqrstuv", 0
ydesc:	.byte "0123456789ABCDEFGHIJKLMNOPQRSTUV", 0

.proc nmi
			inc nmi_count
			rts
.endproc


.proc	irq
			inc irq_count
			lda count
			sta counted
			lda count+1
			sta counted+1
			lda #0
			sta count
			sta count+1
			rts
.endproc

.proc reset
			lda #0
			sta	lcd_x_pos
			sta lcd_y_pos
			cli
			lda	#lcd_mode_y_increment
			sta	lcd_mode

			lda #0
			sta lcd_x
			lda #<xdesc
			ldx	#>xdesc
			ldy #0
			jsr printstring

			lda #lcd_x_pos_plane2_flag
			sta lcd_x
			lda #<xdesc
			ldx	#>xdesc
			ldy #128
			jsr printstring

			lda #0
			sta lcd_x
			lda #<ydesc
			ldx	#>ydesc
			ldy #0
			sty	lcd_y
			jsr printstringy

;			lda #$90;(lcd_x_pos_plane2_flag|(128/8))
			lda #(lcd_x_pos_plane2_flag|(128/8))
			sta lcd_x
			lda #<ydesc
			ldx	#>ydesc
			ldy #0
			sty	lcd_y
			jsr printstringy

			lda #<format
			ldx	#>format
			ldy #8
			jsr printstring

			lda #0
			sta lcd_mode
			lda	#24/8
			sta lcd_x
			lda #24
			sta lcd_y
			lda #'X'
			jsr printsign
			lda #$80
			sta lcd_mode
			lda #32/8
			sta lcd_x
			lda #32
			sta lcd_y
			lda #'Y'
			jsr printsign
			lda #$c0
			sta lcd_mode
			lda #40/8
			sta lcd_x
			lda	#40
			sta lcd_y
			lda #'Z'
			jsr printsign

			lda #0
			sta lcd_mode
			lda	#lcd_x_pos_plane2_flag|(48/8)
			sta lcd_x
			lda #48
			sta lcd_y
			lda #'x'
			jsr printsign
			lda #$80
			sta lcd_mode
			lda	#(lcd_x_pos_plane2_flag|(56/8))
			sta lcd_x
			lda #56
			sta lcd_y
			lda #'y'
			jsr printsign
			lda #$c0
			sta lcd_mode
			lda	#(lcd_x_pos_plane2_flag|(64/8))
			sta lcd_x
			lda	#64
			sta lcd_y
			lda #'z'
			jsr printsign

			lda #lcd_mode_y_increment|1
			sta lcd_mode
			lda	#16/8
			sta lcd_x
			lda #72
			sta lcd_y
			lda #'V'
			jsr printsign
			lda #lcd_mode_y_increment|2
			sta lcd_mode
			lda	#24/8
			sta lcd_x
			lda #72
			sta lcd_y
			lda #'V'
			jsr printsign
			lda #lcd_mode_y_increment|4
			sta lcd_mode
			lda	#32/8
			sta lcd_x
			lda #72
			sta lcd_y
			lda #'V'
			jsr printsign
			lda #lcd_mode_y_increment|8
			sta lcd_mode
			lda	#40/8
			sta lcd_x
			lda #72
			sta lcd_y
			lda #'V'
			jsr printsign


			lda #1
			sta nmi_enable

loop:	lda count
			clc
			adc	#1
			sta count
			lda count+1
			adc	#1
			sta count+1
			lda irq_count
			cmp irq_count
			beq	loop

			jsr inputs
			lda	#lcd_mode_y_increment
			sta	lcd_mode
			jsr printy			



			jmp loop
.endproc

.proc printy
			ldx	#0
			ldy	#16
			lda irq_count
			jsr printhex

			ldx	#3
			ldy	#16
			lda nmi_count
			jsr printhex

			ldx #6
			ldy #16
			lda counted+1
			jsr printhex
			ldx #8
			ldy #16
			lda counted
			jsr printhex

			ldx #11
			ldy #16
			lda xpos
			jsr printhex
			ldx #14
			ldy #16
			lda ypos
			jsr printhex
			rts		
.endproc

.proc inputs
			lda controls
			and #up
			bne notup
			dec	ypos
			lda ypos
			sta lcd_y_pos
notup:lda controls
			and #down
			bne notdown
			inc ypos
			lda ypos
			sta lcd_y_pos
notdown:lda controls
			and #left
			bne notleft
			dec xpos
			lda xpos
			sta lcd_x_pos
notleft:lda controls
			and #right
			bne notright
			inc xpos
			lda xpos
			sta lcd_x_pos
notright:lda controls
			and #start
			bne notstart
notstart:lda controls
			and #select
			bne notselect
notselect:lda controls
			and #button_a
			bne notbuttona
notbuttona:lda controls
			and #button_b
			bne notbuttonb
notbuttonb:rts
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

