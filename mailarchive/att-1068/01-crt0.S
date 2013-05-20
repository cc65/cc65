;
; Startup code for cc65 (NES version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		initlib, donelib
       	.import	       	zerobss, push0
	.import	     	_main
	.import		__STACK_START__, __STACK_SIZE__	; Linker generated
	.import		__DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__
	.import		_memcpy
	.import		pushax
		
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

	.exportzp	_nmi_counter, _irq_counter
_nmi_counter:	.res	1
_irq_counter:	.res	1
.code

; ------------------------------------------------------------------------
; Actual code

	.global	crt0_start
crt0_start:	
reset_vector:
	sei			; disable interrupts
	cld			; binary arithmetic
	ldx	#255		; init stack pointer
	txs
	jsr	zerobss

	;; Set up pointer to stack at top of memory
	lda    	#<(__STACK_START__ + __STACK_SIZE__)
	sta	sp
	lda	#>(__STACK_START__ + __STACK_SIZE__)
       	sta	sp+1   		; Set argument stack ptr

	;; init data segment
	lda	#<__DATA_RUN__
	ldx	#>__DATA_RUN__
	jsr	pushax

	lda	#<__DATA_LOAD__
	ldx	#>__DATA_LOAD__
	jsr	pushax
	
	lda	#<__DATA_SIZE__
	ldx	#>__DATA_SIZE__

	jsr	_memcpy
	
; Call module constructors
	jsr	initlib	

; Pass an empty command line

       	jsr    	push0 	  	; argc
	jsr	push0	  	; argv

	ldy	#4	  	; Argument size
       	jsr    	_main	  	; call the users code

; Call module destructors. This is also the _exit entry.

_exit:
	jmp	_exit

	.segment "VECTORS"
	.word	nmi_vector
	.word	reset_vector
	.word	irq_vector
	
.code
	
nmi_vector:
	inc	_nmi_counter
	rti

irq_vector:
	inc	_irq_counter
	rti

