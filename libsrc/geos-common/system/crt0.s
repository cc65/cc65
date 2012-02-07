;
; Startup code for geos
;
; Maciej 'YTM/Elysium' Witkowiak
; 26.10.99, 10.3.2000, 15.8.2001, 23.12.2002

	.export		_exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

	.import		__VLIR0_START__, __VLIR0_SIZE__	; Linker generated
	.import		__STACKSIZE__			; Linker generated
	.import		initlib, donelib
	.import		callmain
	.import		zerobss
	.importzp	sp
	.include	"jumptab.inc"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment       	"STARTUP"

; Clear the BSS data

	jsr	zerobss

; Setup stack

	lda    	#<(__VLIR0_START__ + __VLIR0_SIZE__ + __STACKSIZE__)
	sta	sp
	lda	#>(__VLIR0_START__ + __VLIR0_SIZE__ + __STACKSIZE__)
       	sta	sp+1   		; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Push arguments and call main()

	cli
       	jsr    	callmain

; Call module destructors.

_exit:	jsr	donelib	 	; Run module destructors

	jmp	EnterDeskTop	; return control to the system
