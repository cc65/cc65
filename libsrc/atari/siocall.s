;
; Christian Groessler, October 2000
;
; this file provides the __sio_call function
; the function does a SIO call, it's not completely
; generic (e.g. transfer size is fixed), it's used
; to save space with _dio_read and _dio_write functions
;
; unsigned char __fastcall__ _sio_call(_driveid_t drive_id,
;				       _sectnum_t sect_num,
;				       void *buffer,
;				       unsigned int sio_val);
; _driveid_t - 8bit
; _sectnum_t - 16bit
; sio_cal is (sio_command | sio_direction << 8)
;

	.export		__sio_call
	.include	"atari.inc"
	.import		popa,popax

.proc	__sio_call

	sta	DCOMND		; set command into DCB
	stx	DSTATS		; set data flow directon
	jsr	popax		; get buffer address
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
	lda	#15
	sta	DTIMLO		; value got from DOS source
	lda	#128
	sta	DBYTLO		; low byte of bytes to transfer	

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
