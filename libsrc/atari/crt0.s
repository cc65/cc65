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
	.constructor	initsp,26

	.import		getargs, argc, argv
	.import		initlib, donelib
       	.import	       	zerobss, pushax
	.import		_main,__filetab,getfd
	.import		__CODE_LOAD__, __BSS_LOAD__

	.include	"atari.inc"
        .include        "../common/_file.inc"

; ------------------------------------------------------------------------
; Define and export the ZP variables for the runtime

	.exportzp	sp, sreg, regsave
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4
	.exportzp	fntemp, regbank, zpspace


.zeropage

zpstart	= *
sp:	      	.res   	2 	; Stack pointer
sreg:	      	.res	2	; Secondary register/high 16 bit for longs
regsave:      	.res	2	; slot to save/restore (E)AX into
ptr1:	      	.res	2
ptr2:	      	.res	2
ptr3:	      	.res	2
ptr4:	      	.res	2
tmp1:	      	.res	1
tmp2:	      	.res	1
tmp3:	      	.res	1
tmp4:	      	.res	1
fntemp:		.res	2	; Pointer to file name
regbank:      	.res	6	; 6 byte register bank

zpspace	= * - zpstart		; Zero page space allocated

.code

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

       	ldx	#zpspace-1
L1:	lda	sp,x
	sta	zpsave,x
	dex
	bpl	L1

; Clear the BSS data

	jsr	zerobss

; setup the stack

	tsx
	stx	spsave

; report memory usage

	lda	APPMHI
	sta	appmsav			; remember old APPMHI value
	lda	APPMHI+1
	sta	appmsav+1

	lda	MEMTOP
	sta	APPMHI			; initialize our APPMHI value
	ldx	MEMTOP+1
	stx	APPMHI+1

; Call module constructors

	jsr	initlib

; set left margin to 0

	lda	LMARGN
	sta	old_lmargin
	lda	#0
	sta	LMARGN

; set keyb to upper/lowercase mode

	ldx	SHFLOK
	stx	old_shflok
	sta	SHFLOK

; Initialize conio stuff

	lda	#$FF
	sta	CH

; set stdio stream handles

	lda	#0
	jsr	getfd
       	sta    	__filetab + (0 * _FILE_size)    ; setup stdin
	lda	#0
	jsr	getfd
	sta	__filetab + (1 * _FILE_size)    ; setup stdout
	lda	#0
	jsr	getfd
	sta	__filetab + (2 * _FILE_size)    ; setup stderr

; Pass command line if present

	jsr	getargs

	lda	argc
	ldx	argc+1
	jsr	pushax 		; argc
	lda	#<argv
	ldx	#>argv
	jsr	pushax 		; argv

	ldy	#4     		; Argument size
	jsr	_main  		; call the users code

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs	       		; Restore stack pointer

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

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
	bpl	L2

; Back to DOS

	rts

; *** end of main startup code

; setup sp

initsp:
	lda	APPMHI
	sta	sp
	lda	APPMHI+1
	sta	sp+1
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
