; get arguments from command line (when DOS supports it)
; and supply function to get default device: char *getdefdev(void);

; Freddy Offenga, 4/21/2000

; SpartaDOS:
; the ZCRNAME routine is only used to get the default drive because
; ZCRNAME has two disadvantages:
; 1. It will convert D: into D1: instead of Dn: (n = default drive)
; 2. It will give a 'no arguments' status if it detects something
;    like Dn: (without filename).

; OS/A+ DOS:
; ZCRNAME is slightly different from SpartaDOS. It will convert D:
; into Dn: where n is the default drive.

MAXARGS	= 16		; max. amount of arguments in arg. table
CL_SIZE = 64		; command line buffer size
SPACE	= 32		; SPACE char.

        .include "atari.inc"
	.export getargs, argc, argv
	.export _getdefdev		; get default device (e.g. "D1:")
	.importzp ptr1

; Get command line

getargs:
	lda	#0
	sta	argc
	sta	argc+1
	sta	argv
	sta	argv+1

	jsr	detect
	bcs	argdos		; carry set = DOS supports arguments
	rts

; Initialize ourcl buffer

argdos:	lda	#ATEOL
	sta	ourcl+CL_SIZE
	 
; Move SpartaDOS command line to our own buffer

	lda	DOSVEC
	clc
	adc	#<LBUF
	sta	ptr1
	lda	DOSVEC+1
	adc	#>LBUF
	sta	ptr1+1

	ldy	#0
cpcl:	lda	(ptr1),y
	sta	ourcl,y
	iny
	cmp	#ATEOL
	beq	movdon
	cpy	#CL_SIZE
	bne	cpcl

movdon:	lda	#0
	sta	ourcl,y		; null terminate behind ATEOL

; Get default device (LBUF will be destroyed!!)

	ldy	#BUFOFF
	lda	#0
	sta	(DOSVEC),y	; reset buffer offset

; Store dummy argument

	ldy	#LBUF
	lda	#'X'
	sta	(DOSVEC),y
	iny
	lda	#ATEOL
	sta	(DOSVEC),y

; One extra store to avoid the buggy sequence from OS/A+ DOS:
; <D><RETURN><:> => drive number = <RETURN>

	iny
	sta	(DOSVEC),y

; Create crunch vector

	ldy	#ZCRNAME+1
	lda	(DOSVEC),y
	sta	crvec+1
	iny
	lda	(DOSVEC),y
	sta	crvec+2

crvec:	jsr	$FFFF		; will be set to crunch vector

; Get default device

	ldy	#COMFNAM	;  COMFNAM is always "Dn:"
	lda	(DOSVEC),y
	sta	defdev
	iny
	lda	(DOSVEC),y
	sta	defdev+1

; Turn command line into argv table

	ldy	#0
eatspc:	lda	ourcl,y		; eat spaces
	cmp	#ATEOL
	beq	finargs
	cmp	#SPACE
	bne	rpar		; begin of argument found
	iny
	cpy	#CL_SIZE
	bne	eatspc
	beq	finargs		; only spaces is no argument

; Store argument vector

rpar:	lda	argc		; low-byte
	asl
	tax			; table index
	tya			; ourcl index
	clc
	adc	#<ourcl
	sta	argv,x
	lda	#>ourcl
	adc	#0
	sta	argv+1,x
	ldx	argc
	inx
	stx	argc
	cpx	#MAXARGS
	beq	finargs

; Skip this arg.

skiparg:
	ldx	ourcl,y
	cpx	#ATEOL		; end of line?
	beq	eopar
	cpx	#SPACE
	beq	eopar
	iny
	cpy	#CL_SIZE
	bne	skiparg

; End of arg. -> place 0

eopar:
	lda	#0
	sta	ourcl,y
	iny			; y behind arg.
	cpx	#ATEOL		; was it the last arg?
	bne	eatspc

; Finish args

finargs:
	lda	argc
	asl
	tax
	lda	#0
	sta	argv,x
	sta	argv+1,x
	rts

; DOS type detection

detect:
	lda	DOS
	cmp	#$53		; "S" (SpartaDOS)
	beq	spdos

	ldy	#COMTAB
	lda	#$4C
	cmp	(DOSVEC),y
	bne	nordos

	ldy	#ZCRNAME
	cmp	(DOSVEC),y
	bne	nordos

	ldy	#6		; OS/A+ has a jmp here
	cmp	(DOSVEC),y
	beq	nordos

spdos:	sec			; SpartaDOS, OS/A+ or DOS XL
	rts

nordos:	clc			; normal DOS (no args) detected
	rts

; Get default device (set by getargs routine)

_getdefdev:
	lda	#<defdev
	ldx	#>defdev
	rts

	.data

; Default device

defdev:
	.byte	"D1:", 0

	.bss

argc:	.res	2
argv:	.res	(1 + MAXARGS) * 2

; Buffer for command line / argv strings

ourcl:	.res	CL_SIZE+1
