;
; Christian Groessler, Dec-2001
;
; int remove (const char* name);
;
UCASE_FILENAME	= 1		; comment it out if filename shouldn't be uppercased

	.include "atari.inc"
	.include "../common/errno.inc"
	.import	findfreeiocb,incsp2,__do_oserror
	.import	popax,__oserror,__seterrno
	.importzp tmp1
.ifdef	UCASE_FILENAME
	.import	addysp,_strupr,subysp
	.importzp tmp3,ptr4,sp
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
	;@@@TODO merge with almost same code in open.s
	; we make sure that the filename doesn't contain lowercase letters
	; we copy the filename we got onto the stack, uppercase it and use this
	; one to open the iocb
	; we're using tmp3, ptr4

	; save the original pointer
	sta	ptr4
	stx	ptr4+1

	; now we need the length of the name
	ldy	#0
loop:	lda	(ptr4),y
	beq	str_end
	cmp	#ATEOL		; we also accept Atari EOF char as end of string
	beq	str_end
	iny
	bne	loop		; not longer than 255 chars (127 real limit)
toolong:lda	#<EINVAL	; file name is too long
	ldx	#>EINVAL
	jmp	seterr

str_end:iny			; room for terminating zero
	cpy	#128		; we only can handle lenght < 128
	bcs	toolong
	sty	tmp3		; save size
	jsr	subysp		; make room on the stack

	; copy filename to the temp. place on the stack
	lda	#0		; end-of-string
	sta	(sp),y		; Y still contains length + 1
	dey
loop2:	lda	(ptr4),y
	sta	(sp),y
	dey
	bpl	loop2		; bpl: this way we only support a max. length of 127

	; uppercase the temp. filename
	ldx	sp+1
	lda	sp
	jsr	_strupr

	; leave X and Y pointing to the modified filename
	lda	sp
	ldx	sp+1

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
;	php
	tya
	pha
	ldy	tmp3		; get size
	jsr	addysp		; free used space on the stack
	pla
	tay
;	plp
.endif	; defined UCASE_FILENAME

	bmi	cioerr

	ldx	#0
	stx	__oserror
	txa
	rts

cioerr:	jmp	__do_oserror

.endproc	; _remove
