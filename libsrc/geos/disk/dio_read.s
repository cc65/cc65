;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
; this file provides the _dio_read function
;
; unsigned char __fastcall__ dio_read(dhandle_t handle,sectnum_t sect_num,void *buffer);
; dhandle_t - 16bit (ptr)
; sectnum_t - 16bit
;

	.import		dio_params
 	.export		_dio_read
	.include "../inc/geossym.inc"
	.include "../inc/jumptab.inc"

.proc	_dio_read

	jsr dio_params
	jsr ReadBlock
	stx errno
	txa
	rts

.endproc
