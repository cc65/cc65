;
; Ullrich von Bassewitz, 23.09.1998
;
; unsigned readjoy (unsigned char joy);
;

     	.export		_readjoy
	.import		sys_bank, restore_bank
	.importzp	cia2, tmp1

	.include	"cbm510.inc"

; ------------------------------------------------------------------------
; unsigned __fastcall__ readjoy (unsigned char joy);


.proc	_readjoy

	jsr	sys_bank    	; Switch to the system bank
       	tax			; Save joystick number

; Get the direction bits

	ldy	#CIA_PRB
	lda	(cia2),y    	; Read joystick inputs
	sta	tmp1

; Get the fire bits

        ldy     #CIA_PRA
	lda	(cia2),y

; Make the result value

	cpx	#$00	    	; Joystick 0?
	bne	@L1 	    	; Jump if no

; Joystick 1, fire is in bit 6, direction in bit 0-3

  	asl     a
	jmp	@L2

; Joystick 2, fire is in bit 7, direction in bit 5-7

@L1:    ldy     #$00  	     	; High byte of return value
	lsr     tmp1
	lsr	tmp1
	lsr	tmp1
       	lsr	tmp1

; Mask the relavant bits, get the fire bit

@L2:	asl	a		; Fire bit into carry
	lda	tmp1
	and	#$0F
    	bcc	@L3
    	ora	#$10
@L3:	eor	#$1F		; All bits are inverted
	jmp	restore_bank

.endproc

