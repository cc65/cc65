;
; Stefan Haubenthal, 2005-12-24
; Based on Christian Groessler
;
; unsigned char _sysmkdir (const char* name, ...);
;

	.include "atari.inc"
	.include "errno.inc"
	.import	findfreeiocb
	.importzp tmp4
.ifdef	UCASE_FILENAME
	.importzp tmp3
	.import	addysp
	.import	ucase_fn
.endif
	.export	__sysmkdir

.proc	__sysmkdir

	pha			; save input parameter
	txa
	pha

	jsr	findfreeiocb
	beq	iocbok		; we found one

	pla
	pla			; fix up stack

	lda	#TMOF		; too many open files
	rts

iocbok:	stx	tmp4		; remember IOCB index
	pla
	tax
	pla			; get argument again

.ifdef	UCASE_FILENAME

	jsr	ucase_fn
	bcc	ucok1

	lda	#182		; see oserror.s
	rts
ucok1:

.endif	; defined UCASE_FILENAME

	ldy	tmp4		; IOCB index
	sta	ICBAL,y		; store pointer to filename
	txa
	sta	ICBAH,y
	tya
	tax
	lda	#34
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
	lda	#0
	rts
cioerr:	tya
	rts

.endproc	; __sysmkdir
