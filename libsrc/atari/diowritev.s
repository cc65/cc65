;
; Christian Groessler, October 2000
;
; this file provides the _dio_write function
;
; unsigned char __fastcall__ _dio_write_verify(_dhandle_t handle,_sectnum_t sect_num,const void *buffer);
; _dhandle_t - 16bit (ptr)
; _sectnum_t - 16bit
;

	.import		__sio_call,pushax
 	.export		__dio_write_verify
	.include	"atari.inc"

.proc	__dio_write_verify

	jsr	pushax		; push buffer address
	ldx	#%10000000	; indicate i/o direction (write)
	lda	#SIO_WRITEV	; write sector command
	jmp	__sio_call	; do the call and return to the user

.endproc

