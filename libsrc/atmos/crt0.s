;
; Startup code for cc65 (Oric version)
;
; By Debrune Jérôme <jede@oric.org> and Ullrich von Bassewitz <uz@cc65.org>
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import		initlib, donelib
	.import	     	push0, _main, zerobss
	.import		__RAM_START__, __RAM_SIZE__	; Linker generated


; ------------------------------------------------------------------------
; Define and export the ZP variables for the Oric runtime

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
; Actual code

; Clear the BSS data

	jsr	zerobss

; Save system stuff and setup the stack

       	tsx
       	stx    	spsave       	; save system stk ptr

	lda    	#<(__RAM_START__ + __RAM_SIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__)
       	sta	sp+1   		; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Pass an empty command line

       	jsr    	push0		; argc
	jsr	push0		; argv

	ldy	#4    		; Argument size
       	jsr    	_main 		; call the users code

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs

; Back to BASIC

	rts

.bss
spsave:	.res	1


