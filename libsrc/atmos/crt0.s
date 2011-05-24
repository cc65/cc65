;
; Startup code for cc65 (Oric version)
;
; By Debrune Jérôme <jede@oric.org> and Ullrich von Bassewitz <uz@cc65.org>
;

	.export		_exit
	.export		__STARTUP__ : absolute = 1	; Mark as startup
	.import		initlib, donelib
	.import		callmain, zerobss
	.import		__RAM_START__, __RAM_SIZE__
	.import		__ZPSAVE_LOAD__, __STACKSIZE__

	.include	"zeropage.inc"
	.include	"atmos.inc"


; ------------------------------------------------------------------------
; Oric tape header

.segment	"TAPEHDR"

	.byte	$16, $16, $16	; Sync bytes
	.byte	$24		; End of header marker

	.byte	$00				; $2B0
	.byte	$00				; $2AF
	.byte	$80				; $2AE Machine code flag
	.byte	$C7				; $2AD Autoload flag
	.dbyt	__ZPSAVE_LOAD__			; $2AB
	.dbyt	__RAM_START__			; $2A9
	.byte	$00				; $2A8
	.byte	((.VERSION >> 8) & $0F) + '0'
	.byte	((.VERSION >> 4) & $0F) + '0'
	.byte	(.VERSION & $0F) + '0'
	.byte	$00				; Zero terminated compiler version

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment	"STARTUP"

; Save the zero page area we're about to use

	ldx	#zpspace-1
L1:	lda	sp,x
	sta	zpsave,x	; Save the zero page locations we need
	dex
	bpl	L1

; Clear the BSS data

	jsr	zerobss

; Unprotect columns 0 and 1

	lda	STATUS
	sta	stsave
	and	#%11011111
	sta	STATUS

; Save system stuff and setup the stack

	tsx
	stx	spsave		; Save system stk ptr

	lda	#<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
	sta	sp+1		; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Push arguments and call main()

	jsr	callmain

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs
	lda	stsave
	sta	STATUS

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
	bpl	L2

; Back to BASIC

	rts

; ------------------------------------------------------------------------
; Data

.segment	"ZPSAVE"

zpsave: .res	zpspace

.bss
spsave: .res	1
stsave: .res	1
