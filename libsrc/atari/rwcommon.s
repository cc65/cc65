;
; common iocb setup routine for read, write
; expects __fastcall__ parameters (int fd, void *buf, int count)
;
	.include "atari.inc"
	.include "errno.inc"
	.importzp tmp2,tmp3
	.import popax
	.import __errno,__oserror
	.import	fdtoiocb

	.export __rwsetup


__rwsetup:

	pha			; push size in stack
	txa
	pha
	jsr	popax		; get buffer address
	pha
	txa
	pha
	jsr	popax		; get handle
	jsr	fdtoiocb	; convert to iocb
	bmi	iocberr		; negative (X=$FF or A>$7F) on error.
	tax
	pla			; store address
	sta	ICBAH,x
	pla
	sta	ICBAL,x
	pla			; store length
	sta	ICBLH,x
	pla
	sta	ICBLL,x
	ora	ICBLH,x		; returns Z if length is 0
	rts

iocberr:pla
	pla
	pla
	pla
	ldx	#$FF		; indicate error + clear ZF
	rts

;
; this routine updates errno.  do a JMP here right after calling
; CIOV.	 we expect status in Y.
;
	.export __do_oserror,__inviocb
__do_oserror:
	sty	__oserror	; save os dependent error code
retminus:
	lda	#$FF
	tax			; return -1
	rts

;
; sets EINVAL error code and returns -1
;

__inviocb:
	lda	#<EINVAL
	jsr	__seterrno
	jmp	retminus	; return -1
