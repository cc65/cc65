;
; Christian Groessler, October 2000
;
; This file provides the __sio_call function
; The function does a SIO call, it's not completely
; generic (e.g. transfer size is fixed), it's used
; to save space with _dio_read and _dio_write functions.
;
; unsigned char __fastcall__ _sio_call(dhandle_t handle,
;				       sectnum_t sect_num,
;				       void *buffer,
;				       unsigned int sio_val);
; dhandle_t - 16bit (ptr)
; sectnum_t - 16bit
; sio_val is (sio_command | sio_direction << 8)
;

	.export		__sio_call
	.include	"atari.inc"
	.import		popa,popax
	.import		sectsizetab,__oserror
	.importzp	ptr1

.proc	__sio_call

	sta	DCOMND		; set command into DCB
	stx	DSTATS		; set data flow directon
	jsr	popax		; get buffer address
	sta	DBUFLO		; set buffer address into DCB
	stx	DBUFHI
	jsr	popax
	sta	DAUX1		; set sector #
	stx	DAUX2

	jsr	popax
	sta	ptr1
	stx	ptr1+1

	ldy	#sst_flag
	lda	(ptr1),y
	and	#128
	beq	_inv_hand	; handle not open or invalid

	ldy	#sst_driveno
	lda	(ptr1),y

	clc
	adc	#1
	sta	DUNIT		; unit number (d1,d2,d3,...)

	ldy	#sst_sectsize
	lda	(ptr1),y
	sta	DBYTLO
	iny
	lda	(ptr1),y
	sta	DBYTHI

	lda	#DISKID		; SIO bus ID of diskette drive
	sta	DDEVIC
	lda	#15
	sta	DTIMLO		; value got from DOS source

	jsr	SIOV		; execute

	ldx	#0
	lda	DSTATS
	bmi	_req_err	; error occurred
	txa			; no error occurred
_req_err:
	sta	__oserror
	rts

_inv_hand:
	ldx	#0
	lda	#BADIOC
	bne	_req_err

.endproc
