;
; Startup code for cc65 (C128 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import	     	__hinit, initconio, doneconio, zerobss
	.import		push0, doatexit, _main

	.include	"c128.inc"
	.include	"../cbm/cbm.inc"

; ------------------------------------------------------------------------
; Define and export the ZP variables for the C64 runtime

	.exportzp	sp, sreg, regsave
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4
	.exportzp	regbank, zpspace

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
regbank:      	.res	6	; 6 byte register bank

zpspace	= * - zpstart		; Zero page space allocated

.code

; ------------------------------------------------------------------------
; BASIC header with a SYS call

	.org	$1BFF
        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
        .byte   $9E,"7181"      ; SYS 7181
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker
	.reloc

; ------------------------------------------------------------------------
; Actual code

 	ldy	#zpspace-1
L1:	lda	sp,y
   	sta	zpsave,y       	; save the zero page locations we need
 	dey
       	bpl	L1

; Close open files

 	jsr	CLRCH

; Switch to second charset

 	lda	#14
 	jsr	BSOUT

; Get the current MMU setting and save it. Set new memory config.

	lda	MMU_CR	 	; Get current memory configuration...
       	pha		 	; ...and save it for later
	lda	#$0E	 	; Bank0 with kernal ROM
	sta	MMU_CR

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

	pla		 	; Get MMU setting
	sta	mmusave

       	tsx
       	stx    	spsave	 	; save system stk ptr

 	lda    	#<$C000
 	sta	sp
	lda	#>$C000
 	sta    	sp+1

; Initialize the heap

 	jsr	__hinit

; Initialize conio stuff

	jsr	initconio

; Pass an empty command line

 	jsr	push0	 	; argc
 	jsr	push0	 	; argv

 	ldy	#4    	 	; Argument size
       	jsr    	_main 	 	; call the users code

; fall thru to exit...

_exit:	jsr	doatexit 	; call exit functions

; Reset the conio stuff

 	jsr	doneconio

; Reset stack and the MMU

	ldx    	spsave 	 	; Patched at runtime
 	txs
       	lda    	mmusave	 	; Patched at runtime
	sta	MMU_CR

; Copy back the zero page stuff

 	ldy	#zpspace-1
L2:	lda	zpsave,y
     	sta	sp,y
     	dey
     	bpl	L2

; Done

     	jmp	RESTOR

.data
zpsave:	.res	zpspace

.bss
spsave:	.res	1
mmusave:.res	1



