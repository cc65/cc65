;
; This must be the *second* file on the linker command line
; (.cvt header must be the *first* one)

; Maciej 'YTM/Elysium' Witkowiak
; 26.10.99, 10.3.2000, 15.8.2001, 23.12.2002

	.import		__RAM_START__, __RAM_SIZE__	; Linker generated
	.import		initlib, donelib
	.import		callmain
	.import		zerobss
	.importzp	sp
	.export		_exit
	.include	"../inc/jumptab.inc"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; Clear the BSS data

	jsr	zerobss

; Setup stack

	lda    	#<(__RAM_START__ + __RAM_SIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__)
       	sta	sp+1   		; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Push arguments and call main()

	cli
       	jsr    	callmain

; Call module destructors.

_exit:	jsr	donelib	 	; Run module destructors

	jmp	EnterDeskTop	; return control to the system
