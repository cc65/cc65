;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
; this file provides the _dio_write function
;
; unsigned char __fastcall__ dio_write_verify(dhandle_t handle,sectnum_t sect_num,const void *buffer);
; dhandle_t - 16bit (ptr)
; sectnum_t - 16bit
;

	.import		dio_params, __oserror
 	.export		_dio_write_verify
	.include "../inc/geossym.inc"
	.include "../inc/jumptab.inc"

.proc	_dio_write_verify

	jsr dio_params
	jsr VerWriteBlock
	stx __oserror
	txa
	rts

.endproc
