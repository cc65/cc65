;
; Startup code for cc65 (C64 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
       	.import	       	__hinit, initconio, zerobss, push0, condes
	.import	     	_main

       	.import	       	__CONSTRUCTOR_TABLE__, __CONSTRUCTOR_COUNT__
	.import		__DESTRUCTOR_TABLE__, __DESTRUCTOR_COUNT__

	.include     	"c64.inc"
	.include     	"../cbm/cbm.inc"

; ------------------------------------------------------------------------
; Define and export the ZP variables for the C64 runtime

	.exportzp   	sp, sreg, regsave
	.exportzp   	ptr1, ptr2, ptr3, ptr4
	.exportzp   	tmp1, tmp2, tmp3, tmp4
	.exportzp   	regbank, zpspace

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

	.org	$7FF
        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
        .byte   $9E,"2061"      ; SYS 2061
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker
	.reloc

; ------------------------------------------------------------------------
; Actual code

	ldx    	#zpspace-1
L1:	lda	sp,x
   	sta	zpsave,x	; Save the zero page locations we need
	dex
       	bpl	L1

; Close open files

	jsr	CLRCH

; Switch to second charset

	lda	#14
	jsr	BSOUT

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

       	tsx
       	stx    	spsave 		; Save the system stack ptr

	lda	$01
	sta	mmusave      	; Save the memory configuration
	and	#$F8
       	ora	#$06		; Enable kernal+I/O, disable basic
	sta	$01

	lda    	#<$D000
	sta	sp
	lda	#>$D000
       	sta	sp+1   		; Set argument stack ptr

; Initialize the heap

	jsr	__hinit

; Initialize conio stuff

	jsr	initconio

; Pass an empty command line

       	jsr    	push0 	  	; argc
	jsr	push0	  	; argv

	ldy	#4	  	; Argument size
       	jsr    	_main	  	; call the users code

; Call module destructors

_exit:	lda	#<__DESTRUCTOR_TABLE__
	ldx	#>__DESTRUCTOR_TABLE__
	ldy	#<(__DESTRUCTOR_COUNT__*2)
	jsr	condes

; Restore system stuff

  	ldx	spsave
	txs	   	  	; Restore stack pointer
       	lda    	mmusave
	sta	$01	  	; Restore memory configuration

; Copy back the zero page stuff

       	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
       	bpl	L2

; Reset changed vectors, back to basic

	jmp	RESTOR


.data

zpsave:	.res	zpspace

.bss

spsave:	.res	1
mmusave:.res	1
