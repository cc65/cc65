;
; Christian Groessler, October 2000
;
; this file provides the _dio_write function
;
; unsigned char __fastcall__ _dio_write(_driveid_t drive_id, _sectnum_t sect_num, void *buffer);
; _driveid_t - 8bit
; _sectnum_t - 16bit
;

	.import		__sio_call,pushax
 	.export		__dio_write
	.include	"atari.inc"

.proc	__dio_write

	jsr	pushax		; push buffer address
	ldx	#%10000000	; indicate i/o direction (write)
	lda	#SIO_WRITE	; write sector command
	jmp	__sio_call	; do the call and return to the user

.endproc

