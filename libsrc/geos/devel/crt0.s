;
; This must be the *second* file on the linker command line
; (.cvt header must be the *first* one)

; Maciej 'YTM/Elysium' Witkowiak
; 26.10.99, 10.3.2000, 15.8.2001

; no __hinit

	.export		_exit
	.import		initlib, donelib
       	.import	       	pushax
	.import		_main
	.import		zerobss

; ------------------------------------------------------------------------
; Define and export the ZP variables for the C64 runtime

	.exportzp	sp, sreg, regsave, regbank
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4

sp     	=      	$72  		; stack pointer
sreg	=  	$74  		; secondary register/high 16 bit for longs
regsave	=      	$76  		; slot to save/restore (E)AX into
ptr1	=	$7A  		;
ptr2	=	$7C
ptr3	=	$7E
ptr4	=	$70
tmp1	=	$fb
tmp2	=	$fc
tmp3	=	$fd
tmp4	=	$fe

regbank =	$a3		; 6 bytes hopefully not used by Kernal

; ------------------------------------------------------------------------

;	.org $0400-508		; $0400 - length of .cvt header
;	.include "cvthead.s"

	.reloc

; ------------------------------------------------------------------------
; Create an empty LOWCODE segment to avoid linker warnings

.segment        "LOWCODE"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; Clear the BSS data

	jsr	zerobss

; Setup stack

	lda    	#<$6000
	sta	sp
	lda	#>$6000
       	sta	sp+1   		; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Pass an empty command line

	lda  	#0
	tax
	jsr	pushax 	 	; argc
	jsr	pushax	 	; argv

	cli
	ldy	#4	 	; Argument size
       	jsr    	_main	 	; call the users code
	jmp	$c1c3		; jump to GEOS MainLoop

; Call module destructors. This is also the _exit entry which must be called
; explicitly by the code.

_exit:	jsr	donelib	 	; Run module destructors

	jmp	$c22c	 	; EnterDeskTop

