;
; Christian Groessler, October 2000
;
; this file provides the _dio_read function
;
; unsigned char __fastcall__ _dio_read(_driveid_t drive_id, _sectnum_t sect_num, void *buffer);
; _driveid_t - 8bit
; _sectnum_t - 16bit
;

	.import		__sio_call,pushax
 	.export		__dio_read
	.include	"atari.inc"

.proc	__dio_read

	jsr	pushax		; push buffer address
	ldx	#%01000000	; direction value
	lda	#SIO_READ	; read sector command
	jmp	__sio_call	; do the call and return to the user

.endproc

