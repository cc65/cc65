;
; Christian Groessler, May-2000
;
; int open(const char *name,int flags,...);
;

	.include "atari.inc"
	.include "fmode.inc"
	.include "errno.inc"
	.export	_open
	.import	clriocb
	.import	fddecusage,newfd
	.import	findfreeiocb
	.import	__do_oserror,__seterrno,incsp4
	.import	ldaxysp,addysp
	.import	__oserror
	.importzp tmp4,tmp2
.ifdef	UCASE_FILENAME
	.importzp tmp3
	.import	ucase_fn
.endif

.proc	_open

	cpy	#4		; correct # of arguments (bytes)?
	beq	parmok		; parameter count ok
	tya			; parm count < 4 shouldn't be needed to be checked
	sec			;	(it generates a c compiler warning)
	sbc	#4
	tay
	jsr	addysp		; fix stack, throw away unused parameters

parmok:	jsr	findfreeiocb
	beq	iocbok		; we found one

	lda	#<EMFILE	; "too many open files"
	ldx	#>EMFILE
seterr:	jsr	__seterrno
	jsr	incsp4		; clean up stack
	lda	#$FF
	tax
	rts			; return -1

	; process the mode argument
	; @@@TODO: append not handled yet!

iocbok:	stx	tmp4
	jsr	clriocb		; init with zero
	ldy	#1
	jsr	ldaxysp		; get mode
	ldx	tmp4
	cmp	#O_RDONLY
	bne	l1
	lda	#OPNIN
set:	sta	ICAX1,x
	bne	cont

l1:	cmp	#O_WRONLY
	bne	l2
	lda	#OPNOT
	bne	set

l2:	; O_RDWR
	lda	#OPNOT|OPNIN
	bne	set

	; process the filename argument

cont:	ldy	#3
	jsr	ldaxysp

.ifdef	UCASE_FILENAME

	jsr	ucase_fn
	bcc	ucok1
	lda	#<EINVAL	; file name is too long
	ldx	#>EINVAL
	jmp	seterr
ucok1:

.endif	; defined UCASE_FILENAME

	ldy	tmp4

	;AX - points to filename
	;Y  - iocb to use, if open needed
	jsr	newfd		; maybe we don't need to open and can reuse an iocb
				; returns fd num to use in tmp2, all regs unchanged
	bcs	doopen		; C set: open needed
	lda	#0		; clears N flag
	beq	finish

doopen:	sta	ICBAL,y
	txa
	sta	ICBAH,y
	ldx	tmp4
	lda	#OPEN
	sta	ICCOM,x
	jsr	CIOV

	; clean up the stack

finish:	php
	txa
	pha
	tya
	pha

.ifdef	UCASE_FILENAME
	ldy	tmp3		; get size
	jsr	addysp		; free used space on the stack
.endif	; defined UCASE_FILENAME

	jsr	incsp4		; clean up stack

	pla
	tay
	pla
	tax
	plp

	bpl	ok
	jsr	fddecusage	; decrement usage counter of fd as open failed
	jmp	__do_oserror

ok:	lda	tmp2		; get fd
	ldx	#0
	stx	__oserror
	rts

.endproc
