;
; Stefan Haubenthal, 2004-10-07
; Based on code from Pu-239
;
; unsigned char __fastcall__ get_tv (void);
; /* Return the video mode the machine is using */
;

	.export		_get_tv

	.include	"vic20.inc"


;--------------------------------------------------------------------------
; _get_tv

.proc	_get_tv

NTSC_LINES = 261
	; detect the system
	lda	#0
	tax
@L0:	ldy	VIC_HLINE
	cpy	#1
	bne	@L0		; wait for line 1
@L1:	ldy	VIC_HLINE
	beq	@L2		; line 0 reached -> NTSC
	cpy	#NTSC_LINES/2+2
	bne	@L1
	lda	#1
@L2:	rts			; system detected: 0 for NTSC, 1 for PAL

.endproc
