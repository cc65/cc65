;
; Christian Groessler, Apr-2000
;
; int read(int fd,void *buf,int count)
;

	.include "atari.inc"
	.import __rwsetup,__do_oserror,__inviocb,__oserror
	.export	_read

_read:	jsr	__rwsetup	; do common setup for read and write
	beq	done		; if size 0, it's a no-op
	cpx	#$FF		; invalid iocb?
	beq	_inviocb
	lda	#GETCHR		; iocb command code
	sta	ICCOM,x
	jsr	CIOV		; read it
	bpl	done
	cpy	#EOFERR		; eof is treated specially
	beq	done
	jmp	__do_oserror	; update errno

done:	lda	ICBLL,x		; buf len lo
	pha			; save
	lda	ICBLH,x		; get buf len hi
	tax			; to X
	lda	#0
	sta	__oserror	; clear system dependend error code
	pla			; get buf len lo
	rts

_inviocb:
	jmp	__inviocb
