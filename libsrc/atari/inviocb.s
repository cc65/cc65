;
; set EINVAL error code and returns -1
;
	.include "errno.inc"
	.import	__retminus

	.export	__inviocb

__inviocb:
	lda	#<EINVAL
	jsr	__seterrno
	jmp	__retminus	; return -1
