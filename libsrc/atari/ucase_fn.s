;
; Christian Groessler, Dec-2001
;
; ucase_fn
; helper routine to convert a string (file name) to uppercase
; used by open.s and remove.s
;
; Calling parameters:
;	AX   - points to filename
; Return parameters:
;	C    - 0/1 for OK/Error (filename too long)
;	AX   - points to uppercased version of the filename on the stack
;	tmp3 - amount of bytes used on the stack (needed for cleanup)
; Uses:
;       ptr4 - scratch pointer used to remember original AX pointer
;
;

	.include	"atari.inc"

.ifdef	DEFAULT_DEVICE
	.importzp tmp2
.ifdef	DYNAMIC_DD
	.import	__defdev
.endif
.endif
	.importzp tmp3,ptr4,sp
	.import	_strupr,subysp
	.export	ucase_fn

.proc   ucase_fn

	; we make sure that the filename doesn't contain lowercase letters
	; we copy the filename we got onto the stack, uppercase it and use this
	; one to open the iocb
	; we're using tmp3, ptr4

	; save the original pointer
	sta	ptr4
	stx	ptr4+1

.ifdef	DEFAULT_DEVICE
	ldy	#1
	sty	tmp2		; initialize flag: device present in passed string
	lda	#':'
	cmp	(ptr4),y
	beq	hasdev
	iny
	cmp	(ptr4),y
	beq	hasdev
	sta	tmp2		; set flag: no device is passed string
hasdev:
.endif

	; now we need the length of the name
	ldy	#0
loop:	lda	(ptr4),y
	beq	str_end
	cmp	#ATEOL		; we also accept Atari EOF char as end of string
	beq	str_end
	iny
	bne	loop		; not longer than 255 chars (127 real limit)
toolong:sec			; indicate error
	rts

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

.ifdef	DEFAULT_DEVICE
	lda	tmp2
	cmp	#1		; was device present in passed string?
	beq	hasdev2		; yes, don't prepend something

	inc	tmp3		; no, prepend "D:"
	inc	tmp3		; adjust stack size used
	ldy	#3
	jsr	subysp		; adjust stack pointer
	ldy	#2
	lda	#':'
	sta	(sp),y		; insert ':'
	dey
.ifdef	DYNAMIC_DD
	lda	__defdev+1
.else
	lda	#'0'+DEFAULT_DEVICE
.endif
	sta	(sp),y		; insert device number
	dey
	lda	#'D'
	sta	(sp),y		; insert 'D'
hasdev2:
.endif
	; uppercase the temp. filename
	ldx	sp+1
	lda	sp
	jsr	_strupr

	; leave X and Y pointing to the modified filename
	lda	sp
	ldx	sp+1
	clc			; indicate success
	rts

.endproc
