;
; Startup code for cc65 (Apple2 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import	   	__hinit
	.import	   	zerobss, push0, doatexit
	.import		_main

	.include	 "apple2.inc"

; ------------------------------------------------------------------------
; Define and export the ZP variables for the C64 runtime

	.exportzp	sp, sreg, regsave
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4
	.exportzp	regbank, zpspace

; These zero page entries overlap with the sweet-16 registers.
; must be changed if sweet-16 is to be supported
sp     	=  	$00  		; stack pointer
sreg	=  	$02  		; secondary register/high 16 bit for longs
regsave	=  	$04  		; slot to save/restore (E)AX into
ptr1	=	$08  		;
ptr2	=	$0A
ptr3	=	$0C
ptr4	=	$0E
tmp1	=	$10
tmp2	=	$11
tmp3	=	$12
tmp4	=	$13
regbank	=	$14		; 6 byte register bank
zpspace	=	$1A		; Zero page space allocated

; ------------------------------------------------------------------------
; Actual code

	ldy	#zpspace-1
L1:	lda	sp,y
   	sta	zpsave,y	; Save the zero page locations we need
	dey
       	bpl	L1

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

       	tsx
       	stx    	spsave 		; Save the system stack ptr

	lda    	#<TOPMEM
	sta	sp
	lda	#>TOPMEM
       	sta	sp+1   		; Set argument stack ptr

; Initialize the heap

	jsr	__hinit

; Initialize conio stuff

	lda	#$ff
	sta	TEXTTYP

; Set up to use Apple ROM $C000-$CFFF

	;; 	sta	USEROM

; Pass an empty command line

	jsr	push0 	 	; argc
	jsr	push0	 	; argv

	ldy	#4	 	; Argument size
       	jsr    	_main	 	; call the users code

; fall thru to exit...

_exit:
	lda	#$ff
	sta	TEXTTYP

	jsr	doatexit 	; call exit functions

	ldx	spsave
	txs	       		; Restore stack pointer

; Copy back the zero page stuff

	ldy	#zpspace-1
L2:	lda	zpsave,y
	sta	sp,y
	dey
       	bpl	L2

; Reset changed vectors, back to basic

	jmp	RESTOR


.data

zpsave:	.res	zpspace

.bss

spsave:	.res	1
