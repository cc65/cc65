;
; common iocb setup routine for read, write
; expects __fastcall__ parameters (int fd, void *buf, int count)
;
	.include "atari.inc"
	.include "errno.inc"
	.importzp tmp2,tmp3
	.import incsp4,ldax0sp,ldaxysp
	.import __errno,__oserror
	.import	fdtoiocb

	.export __rwsetup

__rwsetup:

	sta	tmp2
	stx	tmp3		; remember size
	ldy	#3
	jsr	ldaxysp		; get fd
	jsr	fdtoiocb	; convert to iocb
	bmi	iocberr
	tax
	cpx	#$80		; iocb must be 0...7
	bcs	iocberr
	lda	tmp2
	sta	ICBLL,x
	lda	tmp3		; size hi
	sta	ICBLH,x
	stx	tmp3
	jsr	ldax0sp		; get buf addr
	stx	tmp2
	ldx	tmp3
	sta	ICBAL,x
	lda	tmp2
	sta	ICBAH,x
	jsr	incsp4		; pop args
	lda	ICBLL,x
	ora	ICBLH,x		; return with Z if length was 0
	rts

iocberr:jsr	incsp4		; pop args
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
