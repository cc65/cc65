;
; Ullrich von Bassewitz, 23.09.1998
; Adapted for Vic20 by Steve Schmidtke 05.08.2002
;
; unsigned readjoy (unsigned char joy);
;

     	.export		_readjoy

	.include	"vic20.inc"


.proc	_readjoy

	tax			; Joystick number into X
	bne    	joy2

; Read joystick 1

joy1:	lda     #$7F		
	ldx     #$C3
	sei			; necessary?
	ldy	VIA1_DDRA
	stx     VIA1_DDRA
	ldx	VIA1_JOY
	sty	VIA1_DDRA
	ldy	VIA2_DDRB
	sta     VIA2_DDRB
	lda	VIA2_JOY	; sw3 happens to be the 7th bit (i.e negative)
	sty	VIA2_DDRB
	cli			; necessary?
; all this mess below tries to normalize the returned results (see joystick.h)
; by shifting bits around and inserting switch3 (right).
; easier (and far smaller) would have been just to return the bits where they
; lay and just #ifdef different joystick constants for the Vic20.
	bmi     s3_off		; must stay valid from the VIA2_JOY load
	txa                     ; state: sw3 on, fire ?
	and	#$20		; find out if fire has been pressed
	bne	f_off1
	txa			; state: sw3 on, fire on
	lsr
	lsr
	and	#$07
	eor	#$1F		
	rts
f_off1:	txa                     ; state: sw3 on, fire off
	lsr
	lsr
	and	#$07 
	eor	#$0F		
	rts
s3_off: txa			; state: sw3 off, fire ?
	and	#$20		; find out if fire has been pressed
	bne	f_off2		; yeah, that's just about my sentiment by now
	txa			; state: sw3 off, fire on
	lsr
	lsr
	and	#$07
	eor	#$17		
	rts
f_off2:	txa			; state: sw3 off, fire off
	lsr
	lsr
	and	#$07 
	eor	#$07		
	rts
 
; Read joystick 2 + (just return joy1)

joy2:	jmp	joy1		; stub for more joysticks (pointless?) 

.endproc

