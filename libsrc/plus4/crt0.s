;
; Startup code for cc65 (Plus/4 version)
;
; This must be the *first* file on the linker command line
;

	.export		_exit
	.import	     	__hinit, push0, doatexit, _main
	.import		initconio, doneconio, zerobss

	.include	"plus4.inc"
	.include	"../cbm/cbm.inc"

; ------------------------------------------------------------------------
; Define and export the ZP variables for the C64 runtime

	.exportzp	sp, sreg, regsave
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4
	.exportzp	regbank, zpspace

sp     	=      	$02  	      	; stack pointer
sreg	=  	$04  	      	; secondary register/high 16 bit for longs
regsave	=      	$06  	      	; slot to save/restore (E)AX into
ptr1	=	$0A  	      	;
ptr2	=	$0C
ptr3	=	$0E
ptr4	=	$10
tmp1	=	$12
tmp2	=	$13
tmp3	=	$14
tmp4	=	$15
regbank	=	$16		; 6 byte register bank
zpspace	=	$1A    		; Zero page space allocated

; ------------------------------------------------------------------------
; BASIC header with a SYS call

	.org	$0FFF
        .word   Head            ; Load address
Head:   .word   @Next
        .word   1000            ; Line number
        .byte   $9E,"4109"	; SYS 4109
        .byte   $00             ; End of BASIC line
@Next:  .word   0               ; BASIC end marker
	.reloc

; ------------------------------------------------------------------------
; Actual code

	ldy	#zpspace-1
L1:	lda	sp,y
   	sta	zpsave,y	; save the zero page locations we need
	dey
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
       	stx    	spsave       	; save system stk ptr

	sec
	jsr	MEMTOP		; Get top memory
	cpy	#$80  		; We can only use the low 32K :-(
	bcc	MemOk
	ldy	#$80
	ldx	#$00
MemOk:	stx	sp
      	sty	sp+1  		; set argument stack ptr

; Initialize the heap

	jsr	__hinit

; Initialize conio stuff

	jsr	initconio

; Pass an empty command line

       	jsr    	push0		; argc
	jsr	push0		; argv

	ldy	#4    		; Argument size
       	jsr    	_main 		; call the users code

; fall thru to exit...

_exit:	jsr	doatexit	; call exit functions
	ldx	spsave
	txs

; Reset the conio stuff

	jsr	doneconio

; Copy back the zero page stuff

	ldy	#zpspace-1
L2:	lda	zpsave,y
	sta	sp,y
	dey
       	bpl	L2

; Reset changed vectors

	jmp	RESTOR


.data
zpsave:	.res	zpspace

.bss
spsave:	.res	1


