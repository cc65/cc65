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
	.import		_main,__filetab,getfd
	.import		__CODE_LOAD__, __BSS_LOAD__
	.import		__graphmode_used

	.include	"atari.inc"

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

; report memory usage and initialize stack pointer

	lda	APPMHI
	sta	appmsav
	lda	APPMHI+1
	sta	appmsav+1

	jsr	getmemtop	; adjust for graphics mode to use

	sta	sp
	sta	APPMHI
	stx	sp+1		; Set argument stack ptr
	stx	APPMHI+1

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

; set stdio stream handles

	lda	#0
	jsr	getfd
	sta	__filetab		; setup stdin
	lda	#0
	jsr	getfd
	sta	__filetab + 2		; setup stdout
	lda	#0
	jsr	getfd
	sta	__filetab + 4		; setup stderr

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

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
	bpl	L2

; Back to DOS

	rts

; *** end of main startup code


; calc. upper memory limit to use

.proc	getmemtop

	ldy	__graphmode_used
	beq	ignore		; mode 0 doesn't need adjustment
	cpy	#32
	bcs	ignore		; invalid value

	tya
	asl	a
	tay
	lda	MEMTOP
	sec
	sbc	grmemusage,y
	pha
	lda	MEMTOP+1
	sbc	grmemusage+1,y
	tax
	pla
	rts

ignore:	lda	MEMTOP
	ldx	MEMTOP+1
	rts

.endproc

	.data

zpsave:	.res	zpspace

	.rodata

; memory usage of the different graphics modes (0-31)
; values < 0 of "bytes needed" are mappped to 0
;               bytes needed    ; mode	; val. of MEMTOP
grmemusage:
	.word	0		; 0	; 39967
	.word	0 ;-318		; 1	; 40285
	.word	0 ;-568		; 2	; 40535
	.word	0 ;-558		; 3	; 40525
	.word	0 ;-298		; 4	; 40265
	.word	182		; 5	; 39785
	.word	1182		; 6	; 38785
	.word	3198		; 7	; 36769
	.word	7120		; 8	; 32847
	.word	7146		; 9	; 32821
	.word	7146		; 10	; 32821
	.word	7146		; 11	; 32821
	.word	162		; 12	; 39805
	.word	0 ;-328		; 13	; 40295
	.word	3278		; 14	; 36689
	.word	7120		; 15	; 32847
	.word	0		; 16	; 39967
	.word	0 ;-320		; 17	; 40287
	.word	0 ;-572		; 18	; 40539
	.word	0 ;-560		; 19	; 40527
	.word	0 ;-296		; 20	; 40263
	.word	184		; 21	; 39783
	.word	1192		; 22	; 38775
	.word	3208		; 23	; 36759
	.word	7146		; 24	; 32821
	.word	7146		; 25	; 32821
	.word	7146		; 26	; 32821
	.word	7146		; 27	; 32821
	.word	160		; 28	; 39807
	.word	0 ;-332		; 29	; 40299
	.word	3304		; 30	; 36663
	.word	7146		; 31	; 32821

; the program used to get these values (Atari BASIC):
;  100 FILE=0
;  110 IF FILE=1 THEN OPEN #1,8,0,"D:FREEMEM.OUT"
;  120 IF FILE<>1 THEN OPEN #1,8,0,"E:"
;  200 DIM G(32)
;  210 FOR I=0 TO 32:GRAPHICS I:GOSUB 1000:G(I)=VAL:NEXT I
;  220 GRAPHICS 0
;  230 FOR I=0 TO 31:PRINT #1;I;":",G(I);" - ";G(0)-G(I):NEXT I
;  240 CLOSE #1
;  999 END
;  1000 VAL=PEEK(741)+256*PEEK(742)
;  1010 RETURN

	.bss

spsave:		.res	1
appmsav:	.res	1
old_shflok:	.res	1
old_lmargin:	.res	1

	.segment "AUTOSTRT"
	.word	$02E0
	.word	$02E1
	.word	__CODE_LOAD__ + 1
