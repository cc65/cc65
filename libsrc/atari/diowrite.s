;
; Christian Groessler, October 2000
;
; this file provides the _dio_write function
;
; unsigned char __fastcall__ dio_write(dhandle_t handle,sectnum_t sect_num,const void *buffer);
; dhandle_t - 16bit (ptr)
; sectnum_t - 16bit
;

	.import		__sio_call,pushax
 	.export		_dio_write
	.include	"atari.inc"

.proc	_dio_write

	jsr	pushax		; push buffer address
	ldx	#%10000000	; indicate i/o direction (write)
	lda	#SIO_WRITE	; write sector command
	jmp	__sio_call	; do the call and return to the user

.endproc

