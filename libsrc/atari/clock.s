;
; Ullrich von Bassewitz, 25.07.2000
;
; Implemented using information from Sidney Cadot <sidney@janis.pds.twi.tudelft.nl
;
; clock_t clock (void);
; unsigned _clocks_per_sec (void);
;

      	.export	       	_clock, __clocks_per_sec
	.importzp	sreg

	.include	"atari.inc"


.proc	_clock

	lda	#0  	      	; Byte 3 is always zero
       	sta    	sreg+1
	php			; Save current I flag value
	cli			; Disable interrupts
	lda	RTCLOK		; Read clock
	ldx	RTCLOK+1
	ldy	RTCLOK+2
	plp			; Restore old I bit
	rts

.endproc


.proc	__clocks_per_sec

	lda	#50		; Assume PAL
	ldx	PAL
	beq	@L1
	ldx	#0
	lda	#60
@L1:	rts

.endproc


