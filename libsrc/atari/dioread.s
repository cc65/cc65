;
; Christian Groessler, October 2000
;
; this file provides the _dio_read function
;
; unsigned char __fastcall__ _dio_read(_driveid_t drive_id, _sectnum_t sect_num, void *buffer);
; _driveid_t - 8bit
; _sectnum_t - 16bit
;


 	.export		__dio_read
	.import		popa,popax
	.include	"atari.inc"

.proc	__dio_read

	sta	DBUFLO		; set buffer address into DCB
	stx	DBUFHI
	jsr	popax
	sta	DAUX1		; set sector #
	stx	DAUX2
	jsr	popa
	cmp	#16
	bcs	_inv_dev	; invalid device #
	adc	#1
	sta	DUNIT		; unit number (d1,d2,d3,...)
	lda	#$31		; D1 (drive_id == 0) has id $31
	sta	DDEVIC
	lda	#0
	sta	DBYTHI		; high byte of bytes to transfer
	lda	#%01000000	; indicate i/o direction (read)
	sta	DSTATS
	lda	#15
	sta	DTIMLO		; value got from DOS source
	lda	#128
	sta	DBYTLO		; low byte of bytes to transfer	
	lda	#SIO_READ	; read sector
	sta	DCOMND

	jsr	SIOV		; execute

	ldx	#0
	lda	DSTATS
	bmi	_req_err	; error occurred
	txa			; no error occurred
_req_err:
	rts

_inv_dev:
	ldx	#0
	lda	#NONDEV		; non-existent device error
	rts

.endproc
