;
; Christian Groessler, May-2000
;
; int open(const char *name,int flags,...);
;

UCASE_FILENAME	= 1		; comment it out if filename shouldn't be uppercased

	.include "atari.inc"
	.include "../common/fmode.inc"
	.include "../common/errno.inc"
	.export	_open
	.import	fddecusage,newfd
	.import	__do_oserror,__seterrno,incsp4
	.import	ldaxysp,addysp,subysp
	.import	_strupr,__oserror
	.importzp tmp4,tmp2,sp
.ifdef	UCASE_FILENAME
	.importzp tmp3,ptr4
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


; find a free iocb
; no entry parameters
; return ZF = 0/1 for not found/found
;        index in X if found
; all registers destroyed

.proc	findfreeiocb

	ldx	#0
	ldy	#$FF
loop:	tya
	cmp	ICHID,x
	beq	found
	txa
	clc
	adc	#$10
	tax
	cmp	#$80
	bcc	loop
	inx			; return ZF cleared
found:	rts

.endproc


; clear iocb except for ICHID field
; expects X to be index to IOCB (0,$10,$20,etc.)
; all registers destroyed

.proc	clriocb

	inx			; don't clear ICHID
	ldy	#15
	lda	#0
loop:	sta	ICHID,x
	inx
	dey
	bne	loop
	rts

.endproc
