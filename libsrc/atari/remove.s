;
; Christian Groessler, Dec-2001
;
; int remove (const char* name);
;

	.include "atari.inc"
	.include "../common/errno.inc"
	.import	findfreeiocb,incsp2,__do_oserror
	.import	popax,__oserror,__seterrno
	.importzp tmp1
.ifdef	UCASE_FILENAME
	.importzp tmp3
	.import	addysp
	.import	ucase_fn
.endif
	.export	_remove

.proc	_remove

	jsr	findfreeiocb
	beq	iocbok		; we found one

	jsr	incsp2		; discard stack argument
	lda	#<EMFILE	; "too many open files"
	ldx	#>EMFILE
seterr:	jsr	__seterrno
	lda	#$FF
	tax
	rts			; return -1

iocbok:	;stx	tmp1		; remember IOCB index
	txa
	pha
	jsr	popax

.ifdef	UCASE_FILENAME

	jsr	ucase_fn
	bcc	ucok1
	lda	#<EINVAL	; file name is too long
	ldx	#>EINVAL
	jmp	seterr
ucok1:

.endif	; defined UCASE_FILENAME

	;ldy	tmp1
	sta	tmp1
	pla
	tay
	lda	tmp1
	sty	tmp1
	sta	ICBAL,y
	txa
	sta	ICBAH,y
	ldx	tmp1
	lda	#DELETE
	sta	ICCOM,x
	lda	#0
	sta	ICAX1,x
	sta	ICAX2,x
	sta	ICBLL,x
	sta	ICBLH,x
	jsr	CIOV

.ifdef	UCASE_FILENAME
	tya
	pha
	ldy	tmp3		; get size
	jsr	addysp		; free used space on the stack
	pla
	tay
.endif	; defined UCASE_FILENAME

	bmi	cioerr

	ldx	#0
	stx	__oserror
	txa
	rts

cioerr:	jmp	__do_oserror

.endproc	; _remove
