;
; Startup code for cc65 (ATARI version)
;
; Contributing authors:
;	Mark Keates
;	Freddy Offenga
;	Christian Groessler
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		getargs, argc, argv
	.import		__hinit, initconio, zerobss, pushax, doatexit
	.import		_main,__filetab
	.import		__CODE_LOAD__, __BSS_LOAD__

	.include	"atari.inc"

; ------------------------------------------------------------------------
; Define and export the ZP variables for the runtime

	.exportzp	sp, sreg, regsave
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4
	.exportzp	fntemp, regbank, zpspace

sp	= $D2		; (2bytes) stack pointer
sreg	= $D4		; (2bytes) secondary register/high 16 bit for longs
regsave = $D6		; (4bytes) slot to save/restore (E)AX into
ptr1	= $DA		; (2bytes)
ptr2	= $DC		; (2bytes)
ptr3	= $DE		; (2bytes)
ptr4	= $E0		; (2bytes)
tmp1	= $E2		; (1byte)
tmp2	= $E3		; (1byte)
tmp3	= $E4		; (1byte)
tmp4	= $E5		; (1byte)
fntemp	= $E6		; (2bytes) pointer to file name
regbank = $E8		; (6bytes) 6 byte register bank
zpspace = $EE - sp	; Zero page space allocated

; ------------------------------------------------------------------------
; EXE header

	.segment "EXEHDR"
	.word	$FFFF
	.word	__CODE_LOAD__
	.word	__BSS_LOAD__ - 1
	.code
	.reloc

; ------------------------------------------------------------------------
; Actual code

	rts	; fix for SpartaDOS / OS/A+
		; they first call the entry point from AUTOSTRT and
		; then the load addess (this rts here).
		; We point AUTOSTRT directly after the rts.

; Real entry point:

; Save the zero page locations we need

	ldy	#zpspace-1
L1:	lda	sp,y
	sta	zpsave,y
	dey
	bpl	L1

; Clear the BSS data

	jsr	zerobss

; setup the stack

	tsx
	stx	spsave

; report memory usage and initialize stack pointer

	lda	APPMHI
	sta	appmsav
	lda	APPMHI+1
	sta	appmsav+1
	
	lda	#<$8000
	sta	sp
	sta	APPMHI
	lda	#>$8000
	sta	sp+1		; Set argument stack ptr
	sta	APPMHI+1
	
; set left margin to 0

	lda	LMARGN
	sta	old_lmargin
	lda	#0
	sta	LMARGN

; set keyb to upper/lowercase mode

	ldx	SHFLOK
	stx	old_shflok
	sta	SHFLOK
	
; Initialize the heap

	jsr	__hinit

; Initialize conio stuff

	jsr	initconio

	lda	#$FF
	sta	CH

; ugly hack for now: set stdio stream handles
; all to iocb #0
; until we know where to go with fd<->iocb relation
; this won't stay here!

	lda	#0
	sta	__filetab + 2
	sta	__filetab + 4

; Pass command line if present

	jsr	getargs

	lda	argc
	ldx	argc+1
	jsr	pushax		; argc
	lda	#<argv
	ldx	#>argv
	jsr	pushax		; argv

	ldy	#4		; Argument size
	jsr	_main		; call the users code

; fall thru to exit...

_exit:	jsr	doatexit	; call exit functions

	ldx	spsave
	txs			; Restore stack pointer

; restore left margin

	lda	old_lmargin
	sta	LMARGN

; restore kb mode

	lda	old_shflok
	sta	SHFLOK

; restore APPMHI
	
	lda	appmsav
	sta	APPMHI
	lda	appmsav+1
	sta	APPMHI+1

; Copy back the zero page stuff

	ldy	#zpspace-1
L2:	lda	zpsave,y
	sta	sp,y
	dey
	bpl	L2

; Back to DOS

	rts

.data

zpsave:	.res	zpspace
	
.bss

spsave:		.res	1
appmsav:	.res	1
old_shflok:	.res	1
old_lmargin:	.res	1

	.segment "AUTOSTRT"
	.word	$02E0
	.word	$02E1
	.word	__CODE_LOAD__ + 1
